/*
 * text.h - text geometry
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef TEXT_H
#define TEXT_H

#include "geometry.h"
#include "vertextypes.h"

#include <framework/gpumemoryblock.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <string>

class Font;

class Text : public Geometry
{
public:
	Text(void) = default;
	Text(Font *font, const std::string& text);

	void setFont(Font *font);
	void setText(const std::string& text);

	float width(void) const;
	float height(void) const;

	glm::vec2 boundingBox(void) const;

	void setColour(glm::vec4 colour)
	{
		auto vertices = m_vertices->address();

		// apply colour to all vertices
		for (auto i = 0u; i < m_vertices->count(); ++i)
		{
			vertices[i].colour = colour;
		}
	}

private:
	void generateGeometry(void);
	void doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const;

private:
	Font *m_font{nullptr};
	std::string m_text;
	glm::vec2 m_boundingBox;
	std::unique_ptr<GpuMemoryBlock<ColouredTextureVertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;
};

#endif // TEXT_H
