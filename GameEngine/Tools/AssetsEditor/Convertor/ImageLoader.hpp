#pragma once

#include "AssimpLoader.hpp"
#include <Texture/Texture.hpp>
#include <FreeImagePlus.h>
#include <Utils/BitOperations.hpp>
#include <squish/squish.h>
#include <crunch/inc/crnlib.h>
#include <Geometry/MaterialData.hpp>

#include <thread>
#include <mutex>

namespace AGE
{
	class ImageLoader
	{
	public:

		struct DDS_PIXELFORMAT
		{
			uint32_t dwSize;
			uint32_t dwFlags;
			uint32_t dwFourCC;
			uint32_t dwRGBBitCount;
			uint32_t dwRBitMask;
			uint32_t dwGBitMask;
			uint32_t dwBBitMask;
			uint32_t dwABitMask;
		};

		struct DDS_HEADER
		{
			uint32_t		magicNbr;
			uint32_t        dwSize;
			uint32_t        dwFlags;
			uint32_t        dwHeight;
			uint32_t        dwWidth;
			uint32_t        dwPitchOrLinearSize;
			uint32_t        dwDepth;
			uint32_t        dwMipMapCount;
			uint32_t        dwReserved1[11];
			DDS_PIXELFORMAT	ddspf;
			uint32_t        dwCaps;
			uint32_t        dwCaps2;
			uint32_t        dwCaps3;
			uint32_t        dwCaps4;
			uint32_t        dwReserved2;
		};

		static inline float intensity(RGBQUAD const &pixelColor)
		{
			return (float(pixelColor.rgbRed) / 255.0f);
		}

		static void convertBumpToNormal(fipImage &toConvert, float strength = 2.0f)
		{
			const int w = toConvert.getWidth();
			const int h = toConvert.getHeight();

			std::cout << "convert bump to bitmap" << std::endl;
			for (int y = 0; y < h; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					RGBQUAD topLeft, top, topRight, left, right, bottomLeft, bottom, bottomRight;
					RGBQUAD current;
					
					toConvert.getPixelColor(glm::clamp(x - 1, 0, w - 1), glm::clamp(y - 1, 0, h - 1), &topLeft);
					toConvert.getPixelColor(glm::clamp(x, 0, w - 1), glm::clamp(y - 1, 0, h - 1), &top);
					toConvert.getPixelColor(glm::clamp(x + 1, 0, w - 1), glm::clamp(y - 1, 0, h - 1), &topRight);
					toConvert.getPixelColor(glm::clamp(x - 1, 0, w - 1), glm::clamp(y, 0, h - 1), &left);
					toConvert.getPixelColor(glm::clamp(x + 1, 0, w - 1), glm::clamp(y, 0, h - 1), &right);
					toConvert.getPixelColor(glm::clamp(x - 1, 0, w - 1), glm::clamp(y + 1, 0, h - 1), &bottomLeft);
					toConvert.getPixelColor(glm::clamp(x, 0, w - 1), glm::clamp(y + 1, 0, h - 1), &bottom);
					toConvert.getPixelColor(glm::clamp(x + 1, 0, w - 1), glm::clamp(y + 1, 0, h - 1), &bottomRight);

					toConvert.getPixelColor(x, y, &current);

					const float tl = intensity(topLeft);
					const float t = intensity(top);
					const float tr = intensity(topRight);
					const float l = intensity(left);
					const float r = intensity(right);
					const float bl = intensity(bottomLeft);
					const float b = intensity(bottom);
					const float br = intensity(bottomRight);

					glm::vec3 normal((tr + 2.0f * r + br) - (tl + 2.0f * l + bl),
									 (bl + 2.0f * b + br) - (tl + 2.0f * t + tr),
									 1.0f / strength);

					normal = (glm::normalize(normal) + 1.0f) * 0.5f * 255.0f;

					current.rgbRed = BYTE(normal.r);
					current.rgbGreen = BYTE(normal.g);
					current.rgbBlue = BYTE(normal.b);

					toConvert.setPixelColor(x, y, &current);
				}
			}
		}

		static std::string getFileName(std::string const &path)
		{
			size_t lastSlash = path.find_last_of('/');
			size_t lastBackSlash = path.find_last_of('\\');
			size_t lastPoint = path.find_last_of('.');

			if (lastBackSlash != std::string::npos && lastBackSlash > lastSlash)
				lastSlash = lastBackSlash;
			if (lastSlash == std::string::npos)
				lastSlash = 0;
			if (lastSlash + 1 >= path.size())
				return ("");
			return (path.substr(lastSlash + 1, lastPoint - lastSlash));
		}

		static bool save(AssetDataSet &dataSet)
		{
			if (dataSet.texturesLoaded == false)
				return false;
			while (!dataSet.textures.empty())
			{
				auto t = dataSet.textures.back();
				dataSet.textures.pop_back();

				bool convertToNormal = false;

				for (auto material : dataSet.materials)
				{
					if (!t->rawPath.empty() && getFileName(material->bumpTexPath) == getFileName(t->rawPath))
					{
						convertToNormal = true;
						break;
					}
				}

				auto path = dataSet.rawDirectory.path().string() + "\\" + t->rawPath;

				fipImage image;

				if (!image.load(path.c_str()))
				{
					continue;
				}

				std::cout << "texture path: " << t->rawPath << std::endl;

				t->width = image.getWidth();
				t->height = image.getHeight();
				auto colorType = image.getColorType();
				uint32_t bpp = image.getBitsPerPixel();

				bool toResize = false;
				if (!Bits::isPowerOfTwo(t->width))
				{
					t->width = Bits::roundToHighestPowerOfTwo(t->width);
					toResize = true;
				}
				if (!Bits::isPowerOfTwo(t->height))
				{
					t->height = Bits::roundToHighestPowerOfTwo(t->height);
					toResize = true;
				}
				if (toResize)
				{
					if (!image.rescale(t->width, t->height, FILTER_BICUBIC));
					{
						char a[2];
						a[30000] = '1';
					}
					std::cout << "Texture : " << path << " resized !" << std::endl;
				}

				if (colorType == FIC_RGB || convertToNormal)
				{
					t->format = RGB_DXT1_FORMAT;
					if (bpp > 24)
						t->format = RGBA_DXT5_FORMAT;
				}
				else if (colorType == FIC_RGBALPHA)
				{
					t->format = RGBA_DXT5_FORMAT;
					if (bpp < 32)
						t->format = RGB_DXT1_FORMAT;
				}
				else if (colorType == FIC_MINISBLACK || colorType == FIC_MINISWHITE)
				{
					// for the moment, we wont handle monochromatic images
					t->format = LUM_DXT1_FORMAT;
				}
				else
					assert(false);
				
				// Convert image to RGBA and the to DXT
#if 1

				crn_comp_params params;
				crn_mipmap_params mipmaps;

				params.m_height = t->height;
				params.m_width = t->width;

				bool convertResult = image.convertTo32Bits();
				assert(convertResult);

				if (convertToNormal)
					convertBumpToNormal(image);

				auto imgData = image.accessPixels();

				for (int y = 0; y < image.getHeight(); ++y)
				{
					for (int x = 0; x < image.getWidth(); ++x)
					{
						RGBQUAD color;

						image.getPixelColor(x, y, &color);
						BYTE r = color.rgbRed;
						color.rgbRed = color.rgbBlue;
						color.rgbBlue = r;
						image.setPixelColor(x, y, &color);
					}
				}

				// set the compression params
				if (t->format == RGBA_DXT5_FORMAT)
					params.m_format = crn_format::cCRNFmtDXT5;
				else
					params.m_format = crn_format::cCRNFmtDXT1;
				params.m_file_type = cCRNFileTypeDDS;
				params.m_alpha_component = 0;
				params.m_dxt_quality = cCRNDXTQualityNormal;
				if (t->format == LUM_DXT1_FORMAT)
					params.set_flag(cCRNCompFlagGrayscaleSampling, true);
				params.set_flag(cCRNCompFlagPerceptual, false);
				params.m_pImages[0][0] = (crn_uint32*)imgData;

				// set the mipmaps params
				mipmaps.m_gamma_filtering = false;
				mipmaps.m_mode = cCRNMipModeGenerateMips;

				uint32_t compressedSize;

				uint8_t *compressedData = static_cast<uint8_t*>(crn_compress(params, mipmaps, compressedSize));

				assert(compressedData != NULL);

				// --- End of compression ---

				compressedSize -= sizeof(DDS_HEADER);

				DDS_HEADER *textureHeader = (DDS_HEADER*)compressedData;

				t->mipmapNbr = textureHeader->dwMipMapCount;

				t->data.assign(compressedData + sizeof(DDS_HEADER), compressedData + compressedSize);

#else
				auto imgData = FreeImage_GetBits(image);

				t->data.assign(imgData, imgData + t->height * t->width * t->colorNumber);
#endif

				auto folderPath = std::tr2::sys::path(dataSet.serializedDirectory.path().directory_string() + "\\" + File(t->rawPath).getFolder());

				if (!std::tr2::sys::exists(folderPath) && !std::tr2::sys::create_directories(folderPath))
				{
					std::cerr << "Material convertor error : creating directory" << std::endl;
					return false;
				}
				auto name = dataSet.serializedDirectory.path().directory_string() + "\\" + File(t->rawPath).getFolder() + "\\" + File(t->rawPath).getShortFileName() + ".tage";
				std::ofstream ofs(name, std::ios::trunc | std::ios::binary);
				cereal::PortableBinaryOutputArchive ar(ofs);
				ar(*t);

				std::ofstream ofs2(getFileName(t->rawPath) + "dds", std::ios::trunc | std::ios::binary);

				ofs2.write((char*)compressedData, compressedSize + sizeof(DDS_HEADER));
			}
			return true;
		}

		static bool load(AssetDataSet &dataSet)
		{
			dataSet.texturesLoaded = false;
			if (!dataSet.assimpScene)
			{
				return false;
			}

			if (dataSet.assimpScene->HasTextures())
			{
				for (auto textureIndex = 0; textureIndex < dataSet.assimpScene->mNumTextures; ++textureIndex)
				{
					auto &aiText = dataSet.assimpScene->mTextures[textureIndex];
					unsigned int i = 0;
				}
			}

			for (auto &e : dataSet.texturesPath)
			{
				bool found = false;
				for (auto &f : dataSet.textures)
				{
					if (f->rawPath == e)
					{
						found = true;
						break;
					}
				}
				if (found)
					continue;

				auto t = std::make_shared<TextureData>();
				dataSet.textures.push_back(t);
				t->rawPath = e;
			}
			dataSet.texturesPath.clear();
			if (dataSet.textures.size() == 0)
			{
				std::cerr << "ImageLoader : Image has not been loaded" << std::endl;
				return false;
			}
			dataSet.texturesLoaded = true;
			return true;
		}
	};
}