/*
 * font.cpp - a font for drawing text
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "font.h"
#include "resource.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

namespace
{
	class FreeType
	{
	public:
		static FT_Library& library(void)
		{
			return FreeType::instance()->m_library;
		}

	private:
		FreeType(void)
		{
			FT_Init_FreeType(&m_library);
		}

		~FreeType(void)
		{
			FT_Done_FreeType(m_library);
		}

		static FreeType *instance(void)
		{
			static FreeType freetype;
			return &freetype;
		}

		FT_Library m_library;
	};

#ifdef ENABLE_GLYPH_CACHING
	void cacheGlyphs(CharacterAtlas *atlas, FT_FaceRec_ *face)
	{
		char commonPunctuation[] =
		{
			'.', ',', '!', '"', '/', '-', '(', ')'
		};

		for (char i = 'a'; i <= 'z'; i++)
		{
			auto glyphIndex = FT_Get_Char_Index(face, i);
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			// TODO: check errors
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			atlas->addGlyph(i, face->glyph);
		}

		for (char i = 'A'; i <= 'Z'; i++)
		{
			auto glyphIndex = FT_Get_Char_Index(face, i);
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			// TODO: check errors
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			atlas->addGlyph(i, face->glyph);
		}

		for (char i = '0'; i <= '9'; i++)
		{
			auto glyphIndex = FT_Get_Char_Index(face, i);
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			// TODO: check errors
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			atlas->addGlyph(i, face->glyph);
		}

		for (char i : commonPunctuation)
		{
			auto glyphIndex = FT_Get_Char_Index(face, i);
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			// TODO: check errors
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			atlas->addGlyph(i, face->glyph);
		}
	}
#endif // ENABLE_GLYPH_CACHING
}

Font::Font(void)
{
}

Font::Font(const std::string& file)
	: Font()
{
	setFont(file);
}

Font::~Font(void)
{
	delete m_atlas;
}

bool Font::setFont(const std::string& file)
{
	if (m_atlas != nullptr)
	{
		delete m_atlas;
		m_atlas = nullptr;
	}

	if (m_face != nullptr)
	{
		// TODO: handle this
	}

	m_atlas = new CharacterAtlas;

	auto fileData = Resource::read(file);

	if (!fileData.good)
		return false;

	m_fontData = std::move(fileData.data);

	FT_New_Memory_Face(FreeType::library(), reinterpret_cast<FT_Byte *>(m_fontData.data()), m_fontData.size(), 0, &m_face);
	FT_Set_Char_Size(m_face, 0, 16*64, 960/4.357877685622746f, 544/2.451306198162795);

#ifdef ENABLE_GLYPH_CACHING
	cacheGlyphs(m_atlas, m_face);
#endif // ENABLE_GLYPH_CACHING

	return true;
}

void Font::setPointSize(float size)
{
	FT_Set_Char_Size(m_face, 0, FT_F26Dot6(size*64), 960/4.357877685622746f, 544/2.451306198162795);

	// TODO: implement a clear rather than rebuild expensive object
	if (m_atlas != nullptr)
	{
		delete m_atlas;
	}

	m_atlas = new CharacterAtlas;
#ifdef ENABLE_GLYPH_CACHING
	cacheGlyphs(m_atlas, m_face);
#endif // ENABLE_GLYPH_CACHING
}

bool Font::kerning(void) const
{
	return FT_HAS_KERNING(m_face);
}

glm::vec2 Font::kerningInfo(unsigned int character, unsigned int prev)
{
	glm::vec2 info(0, 0);

	if (!kerning())
		return info;

	FT_Vector dxy;
	FT_Get_Kerning(m_face, FT_Get_Char_Index(m_face, prev), FT_Get_Char_Index(m_face, character), FT_KERNING_DEFAULT, &dxy);

	info.x = dxy.x >> 6;
	info.y = dxy.y >> 6;

	return info;
}

Font::GlyphInfo Font::glyphInfo(unsigned int character)
{
	if (!m_atlas->contains(character))
	{
		auto glyphIndex = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT);
		// TODO: check errors
		FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
		m_atlas->addGlyph(character, m_face->glyph);
	}

	return m_atlas->glyphInfo(character);
}

CharacterAtlas *Font::atlas(void) const
{
	return m_atlas;
}
