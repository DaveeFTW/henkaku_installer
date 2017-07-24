/*
 * font.h - a font for drawing text
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef FONT_H
#define FONT_H

#include "characteratlas.h"

#include <vector>

struct FT_FaceRec_;

class Font
{
	friend class TextRenderer;
	friend class Text;
	
public:
	using GlyphInfo = CharacterAtlas::GlyphInfo;

public:
	Font(void);
	Font(const std::string& file);
	~Font(void);

	bool setFont(const std::string& file);
	void setPointSize(float size);

	bool kerning(void) const;
	glm::vec2 kerningInfo(unsigned int character, unsigned int prev);

	Font::GlyphInfo glyphInfo(unsigned int character);

private:
	CharacterAtlas *atlas(void) const;

private:
	FT_FaceRec_ *m_face{nullptr};
	std::vector<char> m_fontData;
	CharacterAtlas *m_atlas{nullptr};
};

#endif // FONT_H
