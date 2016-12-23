/*
 * circularsegment.h - geometry for a segment of a circle
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CIRCULARSEGMENT_H
#define CIRCULARSEGMENT_H

#include <framework/gpumemoryblock.h>
#include <memory>
#include "geometry.h"

#include <cmath>

template <typename Vertex>
class CircularSegment : public Geometry
{
	static_assert(Vertex::streamCount() == 1, "cannot create CircularSegment with multiple streams!");

public:
	CircularSegment(float radius = 1.f, float angle = 45.f, std::size_t steps = 250);

	void setSteps(std::size_t steps);
	std::size_t steps(void) const;

	void setRadius(float radius);
	float radius(void) const;

	template <typename Colour>
	void setColour(Colour colour);

private:
	void doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const override;

private:
	std::unique_ptr<GpuMemoryBlock<Vertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;

	std::size_t m_steps;
	float m_radius;
	float m_angle;
};

template <typename Vertex>
CircularSegment<Vertex>::CircularSegment(float radius, float angle, std::size_t steps)
	: m_radius(radius)
	, m_angle(angle)
{
	setSteps(steps);
}

template <typename Vertex>
void CircularSegment<Vertex>::setSteps(std::size_t steps)
{
	m_vertices = std::make_unique<GpuMemoryBlock<Vertex>>
	(
		steps+1,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	m_indices = std::make_unique<GpuMemoryBlock<uint16_t>>
	(
		steps+2,
		SCE_GXM_MEMORY_ATTRIB_READ
	);

	m_steps = steps;
	setRadius(m_radius);
}

template <typename Vertex>
std::size_t CircularSegment<Vertex>::steps(void) const
{
	return m_steps;
}

template <typename Vertex>
void CircularSegment<Vertex>::setRadius(float radius)
{
	auto vertices = m_vertices->address();
	auto indices = m_indices->address();
	
	// add centre
	vertices[0].position = glm::vec3(radius, radius, 0.f);
	indices[0] = 0;
	
	constexpr auto pi = 3.141592653589793238462643383279502884;
	auto theta = (2*pi / static_cast<float>(m_steps))/(360.f/m_angle);
	
	// step through rest of circle
	for (auto i = 0u; i < m_steps; ++i)
	{
		// +1 to consider the centre vertex
		vertices[i+1].position = glm::vec3(radius+radius*std::cos(theta*i), radius+radius*std::sin(theta*i), 0.f);
		indices[i+1] = i+1;
	}

	// end fan on first vertex on edge
	indices[m_steps+1] = 0;
	m_radius = radius;
}

template <typename Vertex>
float CircularSegment<Vertex>::radius(void) const
{
	return m_radius;
}

template <typename Vertex>
template <typename Colour>
void CircularSegment<Vertex>::setColour(Colour colour)
{
	auto vertices = m_vertices->address();

	// apply colour to all vertices
	for (auto i = 0u; i < m_vertices->count(); ++i)
	{
		vertices[i].colour = colour;
	}
}

template <typename Vertex>
void CircularSegment<Vertex>::doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const
{
	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), m_indices->count());
}

#endif // CIRCULARSEGMENT_H
