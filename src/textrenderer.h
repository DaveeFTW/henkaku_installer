/*
 * textrenderer.h - text geometry and drawing functionality
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <framework/gpumemoryblock.h>
#include <framework/gxmshaderprogram.h>
#include <framework/gxmvertexshader.h>
#include <framework/gxmfragmentshader.h>

#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

class TextGeometry
{
public:
	struct TextVertex
	{
		float x, y, z;
		float u, v;
	};

public:
	void setVertices(std::vector<TextVertex>&& vertices)
	{
		m_vertices = std::make_unique<GpuMemoryBlock<TextVertex>>
		(
			vertices.size(),
			SCE_GXM_MEMORY_ATTRIB_READ
		);

		std::memcpy(m_vertices->address(), vertices.data(), vertices.size()*sizeof(TextVertex));
	}

	void setIndices(std::vector<uint16_t>&& indices)
	{
		m_indices = std::make_unique<GpuMemoryBlock<uint16_t>>
		(
			indices.size(),
			SCE_GXM_MEMORY_ATTRIB_READ
		);

		std::memcpy(m_indices->address(), indices.data(), indices.size()*sizeof(uint16_t));
	}

	GpuMemoryBlock<TextVertex> *vertices(void) const
	{
		return m_vertices.get();
	}

	GpuMemoryBlock<uint16_t> *indices(void) const
	{
		return m_indices.get();
	}

private:
	std::unique_ptr<GpuMemoryBlock<TextVertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;
};

class Font;

class TextRenderer
{
public:
	TextRenderer(GxmShaderPatcher *patcher);

	void setFont(Font *font);
	void setMvp(glm::mat4 mvp);
	
	TextGeometry *generateTextGeometry(const std::string& text);
	void draw(SceGxmContext *context, TextGeometry *geometry);

private:
	GxmShaderProgram m_program;
	GxmVertexShader m_vertexShader;
	GxmFragmentShader m_fragmentShader;
	GxmShader::UniformIndex m_mvpIndex;
	glm::mat4 m_mvp;
	Font *m_font;
};

#endif // TEXTRENDERER_H
