/*
 * texturerectangle.h - define the geometry of a textured rectangle
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef TEXTURERECTANGLE_H
#define TEXTURERECTANGLE_H

#include <framework/gpumemoryblock.h>
#include <framework/gxmtexture.h>

#include <glm/gtx/transform.hpp>

#include <memory>
#include "geometry.h"

template <typename Vertex>
class TextureRectangle : public Geometry
{
	static_assert(Vertex::streamCount() == 1, "cannot create rectangle with multiple streams!");

public:
	TextureRectangle(void);
	TextureRectangle(float width, float height);
	
	template <typename Colour>
	void setBottomLeftColour(Colour colour);
	template <typename Colour>
	void setBottomRightColour(Colour colour);
	template <typename Colour>
	void setTopLeftColour(Colour colour);
	template <typename Colour>
	void setTopRightColour(Colour colour);

	void setWidth(float width);
	void setHeight(float height);

	template <typename Colour>
	void setColour(Colour colour);

	void setTexture(const GxmTexture *texture);

	float width(void) const { return m_width; }
	float height(void) const { return m_height; }

private:
	void setSize(float width, float height);
	void doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const override;

private:
	std::unique_ptr<GpuMemoryBlock<Vertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;

	Vertex *m_topLeft;
	Vertex *m_topRight;
	Vertex *m_bottomLeft;
	Vertex *m_bottomRight;

	float m_width, m_height;
	glm::mat4 m_model;
	glm::mat4 m_setModel;
	const GxmTexture *m_texture;
};

template <typename Vertex>
TextureRectangle<Vertex>::TextureRectangle(void)
{
	m_vertices = std::make_unique<GpuMemoryBlock<Vertex>>
	(
		4,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	m_indices = std::make_unique<GpuMemoryBlock<uint16_t>>
	(
		6,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	m_bottomLeft = &m_vertices->address()[0];
	m_bottomRight = &m_vertices->address()[1];
	m_topLeft = &m_vertices->address()[2];
	m_topRight = &m_vertices->address()[3];
	
	m_bottomLeft->position = glm::vec3(-1, -1, 0);
	m_bottomRight->position = glm::vec3(1, -1, 0);
	m_topLeft->position = glm::vec3(-1, 1, 0);
	m_topRight->position = glm::vec3(1, 1, 0);

	m_bottomLeft->texCoord = glm::vec2(0, 1);
	m_bottomRight->texCoord = glm::vec2(1, 1);
	m_topLeft->texCoord = glm::vec2(0, 0);
	m_topRight->texCoord = glm::vec2(1, 0);

	m_indices->address()[0] = 0;
	m_indices->address()[1] = 1;
	m_indices->address()[2] = 2;
	m_indices->address()[3] = 1;
	m_indices->address()[4] = 3;
	m_indices->address()[5] = 2;

	setWidth(1.f);
	setHeight(1.f);
}

template <typename Vertex>
TextureRectangle<Vertex>::TextureRectangle(float width, float height)
	: TextureRectangle<Vertex>()
{
	setWidth(width);
	setHeight(height);
}

template <typename Vertex>
void TextureRectangle<Vertex>::setWidth(float width)
{
	setSize(width, height());
}

template <typename Vertex>
void TextureRectangle<Vertex>::setHeight(float height)
{
	setSize(width(), height);
}

template <typename Vertex>
void TextureRectangle<Vertex>::setSize(float width, float height)
{
	m_width = width;
	m_height = height;

	auto scale = glm::scale(glm::mat4(1), glm::vec3(width/2.f, height/2.f, 1.f));
	auto translate = glm::translate(glm::mat4(1), glm::vec3(width/2.f, height/2.f, 0.f));
	setModel(translate, glm::mat4(1), scale);
}

template <typename Vertex>
template <typename Colour>
void TextureRectangle<Vertex>::setBottomLeftColour(Colour colour)
{
	m_bottomLeft->colour = colour;
}

template <typename Vertex>
template <typename Colour>
void TextureRectangle<Vertex>::setBottomRightColour(Colour colour)
{
	m_bottomRight->colour = colour;
}

template <typename Vertex>
template <typename Colour>
void TextureRectangle<Vertex>::setTopLeftColour(Colour colour)
{
	m_topLeft->colour = colour;
}

template <typename Vertex>
template <typename Colour>
void TextureRectangle<Vertex>::setTopRightColour(Colour colour)
{
	m_topRight->colour = colour;
}

template <typename Vertex>
template <typename Colour>
void TextureRectangle<Vertex>::setColour(Colour colour)
{
	auto vertices = m_vertices->address();

	// apply colour to all vertices
	for (auto i = 0u; i < m_vertices->count(); ++i)
	{
		vertices[i].colour = colour;
	}
}

template <typename Vertex>
void TextureRectangle<Vertex>::setTexture(const GxmTexture *texture)
{
	m_texture = texture;
}

template <typename Vertex>
void TextureRectangle<Vertex>::doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const
{
	m_texture->bind(ctx, 0);
	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), m_indices->count());
}

#endif // TEXTURERECTANGLE_H
