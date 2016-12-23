/*
 * characteratlas.cpp - map unicode characters to glyph regions
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "characteratlas.h"

#include <freetype2/ft2build.h>
#include FT_IMAGE_H
#include FT_FREETYPE_H

CharacterAtlas::CharacterAtlas(void)
{
	m_atlas = new TextureAtlas(512, 512);
	m_atlas->setFormat(GxmTexture::U8_R111);
	m_atlas->create(GxmTexture::Point, GxmTexture::Linear);
}

CharacterAtlas::~CharacterAtlas(void)
{
	delete m_atlas;
}

bool CharacterAtlas::contains(unsigned int character)
{
	return m_glyphMap.find(character) != m_glyphMap.end();
}

bool CharacterAtlas::addGlyph(unsigned int character, const FT_GlyphSlotRec_ *glyph)
{
	const FT_Bitmap *bitmap = &glyph->bitmap;

	// we cant easily remove characters from our atlas, so we'll just 
	// create a new entry and throw away the old reference
	auto region = m_atlas->region(bitmap->width, bitmap->rows);

	// check if we successfully allocated memory within the atlas
	if (region.x == -1 || region.y == -1)
	{
		return false;
	}
	
	m_atlas->setRegion(region, reinterpret_cast<char *>(bitmap->buffer), bitmap->pitch);
	m_glyphMap.insert({character, 
	{
		region, 
		glm::vec2(glyph->linearHoriAdvance >> 16, glyph->linearVertAdvance >> 16),
		static_cast<float>(glyph->bitmap_left),
		static_cast<float>(glyph->bitmap_top)
	}});
	return true;
}

CharacterAtlas::GlyphInfo CharacterAtlas::glyphInfo(unsigned int character)
{
	GlyphInfo info;
	
	if (!contains(character))
		return info;

	auto glyph = m_glyphMap.at(character);
	info.quad = m_atlas->toQuad(glyph.region);
	info.advance = glyph.advance;
	info.bitmap_left = glyph.bitmap_left;
	info.bitmap_top = glyph.bitmap_top;
	return info;
}

void CharacterAtlas::bind(SceGxmContext *ctx, int unit)
{
	m_atlas->bind(ctx, unit);
}
