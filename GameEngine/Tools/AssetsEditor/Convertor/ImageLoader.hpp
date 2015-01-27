#pragma once

#include "AssimpLoader.hpp"
#include <Texture/Texture.hpp>
#include <FreeImagePlus.h>
#include <Utils/BitOperations.hpp>
#include "ConvertorStatusManager.hpp"
#include "CookingTask.hpp"
namespace AGE
{
	class ImageLoader
	{
	public:
		static bool save(std::shared_ptr<CookingTask> cookingTask)
		{
			auto tid = Singleton<AGE::AE::ConvertorStatusManager>::getInstance()->PushTask("ImageLoader : load and save " + cookingTask->dataSet->filePath.getShortFileName());
			while (!cookingTask->textures.empty())
			{
				auto t = cookingTask->textures.back();
				cookingTask->textures.pop_back();
				auto path = cookingTask->rawDirectory.path().string() + "\\" + t->rawPath;

				fipImage image;

				if (!image.load(path.c_str()))
				{
					continue;
				}

				t->width = image.getWidth();
				t->height = image.getHeight();
				auto colorType = image.getColorType();
				t->bpp = image.getBitsPerPixel();

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
						//ERROR PAUL WILL FIXE ALL THAT PART :D
					}
					std::cout << "Texture : " << path << " resized !" << std::endl;
				}

				t->colorNumber = 0;
				if (colorType == FIC_RGB)
				{
					t->colorNumber = 3;
					if (t->bpp > 24)
						t->colorNumber = 4;
				}
				else if (colorType == FIC_RGBALPHA)
				{
					t->colorNumber = 4;
					if (t->bpp < 32)
						t->colorNumber = 3;
				}
				else if (colorType == FIC_MINISBLACK || colorType == FIC_MINISWHITE)
				{
					t->colorNumber = 1;
				}
				else
					assert(false);
				auto imgData = FreeImage_GetBits(image);
				t->data.assign(imgData, imgData + sizeof(unsigned char) * t->width * t->height * t->colorNumber);

				auto folderPath = std::tr2::sys::path(cookingTask->serializedDirectory.path().directory_string() + "\\" + File(t->rawPath).getFolder());

				if (!std::tr2::sys::exists(folderPath) && !std::tr2::sys::create_directories(folderPath))
				{
					Singleton<AGE::AE::ConvertorStatusManager>::getInstance()->PopTask(tid);
					std::cerr << "Material convertor error : creating directory" << std::endl;
					return false;
				}
				auto name = cookingTask->serializedDirectory.path().directory_string() + "\\" + File(t->rawPath).getFolder() + "\\" + File(t->rawPath).getShortFileName() + ".tage";
				std::ofstream ofs(name, std::ios::trunc | std::ios::binary);
				cereal::PortableBinaryOutputArchive ar(ofs);
				ar(*t);
			}
			Singleton<AGE::AE::ConvertorStatusManager>::getInstance()->PopTask(tid);
			return true;
		}

		static bool load(std::shared_ptr<CookingTask> cookingTask)
		{
			if (cookingTask->assimpScene->HasTextures())
			{
				for (auto textureIndex = 0; textureIndex < cookingTask->assimpScene->mNumTextures; ++textureIndex)
				{
					auto &aiText = cookingTask->assimpScene->mTextures[textureIndex];
					unsigned int i = 0;
				}
			}

			for (auto &e : cookingTask->texturesPath)
			{
				bool found = false;
				for (auto &f : cookingTask->textures)
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
				cookingTask->textures.push_back(t);
				t->rawPath = e;
			}
			cookingTask->texturesPath.clear();
			if (cookingTask->textures.size() == 0)
			{
				std::cerr << "ImageLoader : Image has not been loaded" << std::endl;
				return false;
			}
			return true;
		}
	};
}