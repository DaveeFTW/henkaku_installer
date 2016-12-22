/*
 * rectangle.h - define the geometry of a rectangle
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <framework/gpumemoryblock.h>
#include <memory>
#include "geometry.h"

template <typename Vertex>
class Rectangle : public Geometry
{
	static_assert(Vertex::streamCount() == 1, "cannot create rectangle with multiple streams!");

public:
	Rectangle(void);
	Rectangle(float x, float y, float z, float width, float height);

	void setBottomLeft(const Vertex& vertex);
	void setBottomRight(const Vertex& vertex);
	void setTopLeft(const Vertex& vertex);
	void setTopRight(const Vertex& vertex);

private:
	void doDraw(SceGxmContext *ctx) const override;

private:
	std::unique_ptr<GpuMemoryBlock<Vertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;

	Vertex *m_topLeft;
	Vertex *m_topRight;
	Vertex *m_bottomLeft;
	Vertex *m_bottomRight;
};

template <typename Vertex>
Rectangle<Vertex>::Rectangle(void)
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

	m_indices->address()[0] = 0;
	m_indices->address()[1] = 1;
	m_indices->address()[2] = 2;
	m_indices->address()[3] = 1;
	m_indices->address()[4] = 3;
	m_indices->address()[5] = 2;
}

template <typename Vertex>
Rectangle<Vertex>::Rectangle(float x, float y, float z, float width, float height)
	: Rectangle<Vertex>()
{
	m_bottomLeft->position = glm::vec3(x, y, z);
	m_bottomRight->position = glm::vec3(x+width, y, z);
	m_topLeft->position = glm::vec3(x, y+height, z);
	m_topRight->position = glm::vec3(x+width, y+height, z);
}

template <typename Vertex>
void Rectangle<Vertex>::setBottomLeft(const Vertex& vertex)
{
	*m_bottomLeft = vertex;
}

template <typename Vertex>
void Rectangle<Vertex>::setBottomRight(const Vertex& vertex)
{
	*m_bottomRight = vertex;
}

template <typename Vertex>
void Rectangle<Vertex>::setTopLeft(const Vertex& vertex)
{
	*m_topLeft = vertex;
}

template <typename Vertex>
void Rectangle<Vertex>::setTopRight(const Vertex& vertex)
{
	*m_topRight = vertex;
}

template <typename Vertex>
void Rectangle<Vertex>::doDraw(SceGxmContext *ctx) const
{
	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), m_indices->count());
}

#endif // RECTANGLE_H
