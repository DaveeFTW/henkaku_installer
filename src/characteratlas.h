/*
 * characteratlas.h - map unicode characters to glyph regions
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CHARACTERATLAS_H
#define CHARACTERATLAS_H

#include "textureatlas.h"

#include <glm/vec2.hpp>

#include <unordered_map>

struct FT_GlyphSlotRec_;

class CharacterAtlas
{
public:
	struct GlyphInfo
	{
		TextureAtlas::Quad quad;
		glm::vec2 advance;
	};

public:
	CharacterAtlas(void);
	~CharacterAtlas(void);

	bool contains(unsigned int character);
	bool addGlyph(unsigned int character, const FT_GlyphSlotRec_ *glyph);
	GlyphInfo glyphInfo(unsigned int character);

	void bind(SceGxmContext *ctx, int unit);

private:
	struct GlyphData
	{
		TextureAtlas::AtlasRegion region;
		glm::vec2 advance;
	};

private:
	TextureAtlas *m_atlas;
	std::unordered_map<unsigned int, GlyphData> m_glyphMap;
};

#endif // CHARACTERATLAS_H
