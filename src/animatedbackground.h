/*
 * animatedbackground.h - display and control animated background surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
 
#ifndef ANIMATEDBACKGROUND_H
#define ANIMATEDBACKGROUND_H

#include <framework/gxmtexture.h>
#include <framework/gpumemoryblock.h>
#include <framework/gxmfragmentshader.h>
#include <framework/gxmvertexshader.h>
#include <framework/gxmshaderprogram.h>

struct SceGxmContext;
class Camera;

class AnimatedBackground
{
public:
	AnimatedBackground(GxmShaderPatcher *patcher);

	void update(const Camera *camera, float dt);
	void draw(SceGxmContext *ctx);

	void setColour(glm::vec3 topleft, glm::vec3 bottomRight);

private:
	using Vertex = struct
	{
		glm::vec3 position;
		glm::vec3 color;

		constexpr static std::size_t size(void) { return sizeof(float)*6; }
	};

	using TextureCoord = struct
	{
		float u;
		float v;
	};
	
	using BgTexture = struct
	{
		GxmTexture texture;
		glm::vec2 position;
		glm::vec2 dispRate;
	};

private:
	void loadTexture(GxmTexture *texture, const char *file);

private:
	GxmShaderProgram m_program;
	GxmVertexShader m_vertex;
	GxmFragmentShader m_fragment;
	GxmShader::UniformIndex m_mvpIndex;
	BgTexture m_textures[5];
	glm::vec3 m_colourTopLeft, m_colourBottomRight;
	std::unique_ptr<GpuMemoryBlock<Vertex>> m_vertices;
	std::unique_ptr<GpuMemoryBlock<TextureCoord>> m_texCoords;
	std::unique_ptr<GpuMemoryBlock<uint16_t>> m_indices;
	bool m_animateColours;
	glm::mat4 m_mvp;
};

#endif // ANIMATEDBACKGROUND_H
