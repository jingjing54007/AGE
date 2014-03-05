#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/array.hpp>

#define ASCII_BEGIN 32   //space
#define ASCII_END   127  //DEL
#define GLYPH_PADDING 4

#include <array>
#include <cstddef>
#include <ft2build.h>
#include FT_FREETYPE_H

struct FontGlyph
{
	FontGlyph()
		: index(0)
		, width(0)
		, height(0)
	{
		uvs.fill(0);
	}

	std::size_t index;
	std::size_t width;
	std::size_t height;
	std::array<float, 4> uvs;

	template <typename Archive>
	void serialize(Archive &ar)
	{
		ar(index, width, height, uvs);
	}

};