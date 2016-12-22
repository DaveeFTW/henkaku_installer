/*
 * circle.h - define the geometry of a circle
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CIRCLE_H
#define CIRCLE_H

#include <framework/gpumemoryblock.h>
#include <memory>
#include "geometry.h"

#include <cmath>

template <typename Vertex>
class Circle : public Geometry
{
	static_assert(Vertex::streamCount() == 1, "cannot create Circle with multiple streams!");

public:
	Circle(float radius = 1.f, std::size_t steps = 250);

	void setSteps(std::size_t steps);
	std::size_t steps(void) const;

	void setRadius(float radius);
	float radius(void) const;

private:
	void doDraw(SceGxmContext *ctx) const override;

private:
	std::unique_ptr<GpuMemoryBlock<Vertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;

	std::size_t m_steps;
	float m_radius;
};

template <typename Vertex>
Circle<Vertex>::Circle(float radius, std::size_t steps)
	: m_radius(radius)
{
	setSteps(steps);
}

template <typename Vertex>
void Circle<Vertex>::setSteps(std::size_t steps)
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
std::size_t Circle<Vertex>::steps(void) const
{
	return m_steps;
}

template <typename Vertex>
void Circle<Vertex>::setRadius(float radius)
{
	auto vertices = m_vertices->address();
	auto indices = m_indices->address();
	
	// add centre
	vertices[0].position = glm::vec3(radius, radius, 0.f);
	indices[0] = 0;
	
	constexpr auto pi = 3.141592653589793238462643383279502884;
	auto theta = 2*pi / static_cast<float>(m_steps);
	
	// step through rest of circle
	for (auto i = 0u; i < m_steps; ++i)
	{
		// +1 to consider the centre vertex
		vertices[i+1].position = glm::vec3(radius+radius*std::cos(theta*i), radius+radius*std::sin(theta*i), 0.f);
		indices[i+1] = i+1;
	}

	// end fan on first vertex on edge
	indices[m_steps+1] = 1;
	m_radius = radius;
}

template <typename Vertex>
float Circle<Vertex>::radius(void) const
{
	return m_radius;
}

template <typename Vertex>
void Circle<Vertex>::doDraw(SceGxmContext *ctx) const
{
	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), m_indices->count());
}

#endif // CIRCLE_H
