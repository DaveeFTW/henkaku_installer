/*
 * text.cpp - text geometry
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "text.h"
#include "font.h"

#include <utf8cpp/utf8.h>

Text::Text(Font *font, const std::string& text)
{
	m_text = text;
	m_font = font;
	generateGeometry();
}

void Text::setFont(Font *font)
{
	m_font = font;
	generateGeometry();
}

void Text::setText(const std::string& text)
{
	m_text = text;
	generateGeometry();
}

float Text::width(void) const
{
	return m_boundingBox.x;
}

float Text::height(void) const
{
	return m_boundingBox.y;
}

glm::vec2 Text::boundingBox(void) const
{
	return m_boundingBox;
}

void Text::generateGeometry(void)
{
	utf8::iterator<std::string::const_iterator> it(m_text.begin(), m_text.begin(), m_text.end());
	utf8::iterator<std::string::const_iterator> end(m_text.end(), m_text.begin(), m_text.end());

	m_vertices = std::make_unique<GpuMemoryBlock<ColouredTextVertex>>
	(
		utf8::distance(m_text.begin(), m_text.end())*4,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	m_indices = std::make_unique<GpuMemoryBlock<uint16_t>>
	(
		utf8::distance(m_text.begin(), m_text.end())*3*2,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	auto vertices = m_vertices->address();
	auto indices = m_indices->address();

	int x = 0;
	int y = 0;
	int heightMax = 0;
	std::size_t indiceCount = 0;

	for (auto i = 0; it != end; ++it)
	{
		auto glyphInfo = m_font->glyphInfo(*it);

		// bottom left
		vertices[i+0].position.x = x;
		vertices[i+0].position.y = y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top);
		vertices[i+0].position.z = 0;
		vertices[i+0].texCoord = glyphInfo.quad.bl;

		// bottom right
		vertices[i+1].position.x = x+glyphInfo.quad.size.x;
		vertices[i+1].position.y = y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top);
		vertices[i+1].position.z = 0;
		vertices[i+1].texCoord = glyphInfo.quad.br;

		// top left
		vertices[i+2].position.x = x;
		vertices[i+2].position.y = y+glyphInfo.quad.size.y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top);
		vertices[i+2].position.z = 0;
		vertices[i+2].texCoord = glyphInfo.quad.tl;

		// top right
		vertices[i+3].position.x = x+glyphInfo.quad.size.x;
		vertices[i+3].position.y = y+glyphInfo.quad.size.y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top);
		vertices[i+3].position.z = 0;
		vertices[i+3].texCoord = glyphInfo.quad.tr;
		
		indices[indiceCount+0] = i+0;
		indices[indiceCount+1] = i+1;
		indices[indiceCount+2] = i+2;
		indices[indiceCount+3] = i+1;
		indices[indiceCount+4] = i+3;
		indices[indiceCount+5] = i+2;

		i += 4;
		indiceCount += 6;

		x += glyphInfo.advance.x;

		if (y+glyphInfo.quad.size.y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top) > heightMax)
			heightMax = y+glyphInfo.quad.size.y-(glyphInfo.quad.size.y - glyphInfo.bitmap_top);
	}

	m_boundingBox = glm::vec2(x, y+heightMax);
}

void Text::doDraw(SceGxmContext *ctx, GeometryRenderer *renderer, const Camera *camera) const
{
	m_font->atlas()->bind(ctx, 0);
	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), m_indices->count());
}
