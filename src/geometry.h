/*
 * geometry.h - define simple base class for geometry
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glm/mat4x4.hpp>

struct SceGxmContext;

class Geometry
{
public:
	Geometry(void)
	{
		m_model = glm::mat4(1);
	}

	void setModel(glm::mat4 model)
	{
		m_model = model;
	}

	glm::mat4 modelMatrix(void) const
	{
		return m_model;
	}

	virtual void draw(SceGxmContext *ctx) const = 0;

private:
	glm::mat4 m_model;
};

#endif // GEOMETRY_H

/*class Geometry
{
public:
	GpuMemoryBlock<Vertex> *vertices(void) const
	{
		return m_vertices.get();
	}

	GpuMemoryBlock<uint16_t> *indices(void) const
	{
		return m_indices.get();
	}
};

class Rectangle
{
public:
	struct RectangleVertex
	{
		glm::vec3 position;
		glm::vec4 colour;
	};

public:
	Rectangle(void);
	Rectangle(float x, float y, float width, float height);

	void draw(SceGxmContext *ctx) override;

private:
	RectangleVertex *m_topLeft;
	RectangleVertex *m_topRight;
	RectangleVertex *m_bottomLeft;
	RectangleVertex *m_bottomRight;
};

struct VertexPosition
{
	glm::vec3 position;
};

struct PositionVertex
{
	
};









void GeometryRenderer::draw(SceGxmContext *ctx, Geometry *geometry)
{
	m_program.bind(ctx);

	void *uniform = nullptr;
	sceGxmReserveVertexDefaultUniformBuffer(ctx, &uniform);
	m_vertexShader.setUniformBuffer(uniform);
	m_vertexShader.setUniformValue(m_mvpIndex, camera->projectionMatrix() * camera->viewMatrix() * geometry->modelMatrix());

	geometry->draw(ctx);
}*/
