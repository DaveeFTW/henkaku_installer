/*
 * animatedbackground.cpp - display and control animated background surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "animatedbackground.h"
#include "camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>

#include <psp2/gxm.h>

extern "C"
{
	extern unsigned char animbg_vert_gxp[];
	extern unsigned int animbg_vert_gxp_len;
	extern unsigned char animbg_frag_gxp[];
	extern unsigned int animbg_frag_gxp_len;
}

AnimatedBackground::AnimatedBackground(GxmShaderPatcher *patcher)
	: m_program(patcher)
	, m_vertices(std::make_unique<GpuMemoryBlock<Vertex>>
	(
		4,
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_texCoords(std::make_unique<GpuMemoryBlock<TextureCoord>>
	(
		5*4,
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_indices(std::make_unique<GpuMemoryBlock<uint16_t>>
	(
		6,
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_animateColours(false)
{
	m_vertex.loadFromBuffer(animbg_vert_gxp, animbg_vert_gxp_len);
	m_fragment.loadFromBuffer(animbg_frag_gxp, animbg_frag_gxp_len);

	m_program.addShader(&m_vertex);
	m_program.addShader(&m_fragment);

	auto positionIndex = m_vertex.attributeIndex("position");
	auto colorIndex = m_vertex.attributeIndex("color");
	auto textureIndex1 = m_vertex.attributeIndex("texCoord1");
	auto textureIndex2 = m_vertex.attributeIndex("texCoord2");
	auto textureIndex3 = m_vertex.attributeIndex("texCoord3");
	auto textureIndex4 = m_vertex.attributeIndex("texCoord4");
	auto textureIndex5 = m_vertex.attributeIndex("texCoord5");
	m_mvpIndex = m_vertex.uniformIndex("mvp");

	SceGxmVertexAttribute attributes[7];
	SceGxmVertexStream streams[2];

	attributes[0].streamIndex = 0;
	attributes[0].offset = 0;
	attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[0].componentCount = 3;
	attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(positionIndex);

	attributes[1].streamIndex = 0;
	attributes[1].offset = 3*4;
	attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[1].componentCount = 3;
	attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(colorIndex);

	attributes[2].streamIndex = 1;
	attributes[2].offset = 0;
	attributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[2].componentCount = 2;
	attributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(textureIndex1);

	attributes[3].streamIndex = 1;
	attributes[3].offset = sizeof(TextureCoord);
	attributes[3].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[3].componentCount = 2;
	attributes[3].regIndex = sceGxmProgramParameterGetResourceIndex(textureIndex2);

	attributes[4].streamIndex = 1;
	attributes[4].offset = 2*sizeof(TextureCoord);
	attributes[4].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[4].componentCount = 2;
	attributes[4].regIndex = sceGxmProgramParameterGetResourceIndex(textureIndex3);

	attributes[5].streamIndex = 1;
	attributes[5].offset = 3*sizeof(TextureCoord);
	attributes[5].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[5].componentCount = 2;
	attributes[5].regIndex = sceGxmProgramParameterGetResourceIndex(textureIndex4);

	attributes[6].streamIndex = 1;
	attributes[6].offset = 4*sizeof(TextureCoord);
	attributes[6].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[6].componentCount = 2;
	attributes[6].regIndex = sceGxmProgramParameterGetResourceIndex(textureIndex5);

	streams[0].stride = Vertex::size();
	streams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
	streams[1].stride = 5*sizeof(TextureCoord);
	streams[1].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	m_program.setVertexAttributeFormat(attributes, 7);
	m_program.setVertexStreamFormat(streams, 2);
	m_program.link();

	auto bottomRightRgb = glm::vec3(69.f/255.f, 218.f/255.f, 255.f/255.f);
	auto topLeftRgb = glm::vec3(70.f/255.f, 108.f/255.f, 191.f/255.f);
	
	m_colourTopLeft = glm::hsvColor(topLeftRgb);
	m_colourBottomRight = glm::hsvColor(bottomRightRgb);

	auto interp = 0.5f*topLeftRgb + 0.5f*bottomRightRgb;
	
	Vertex vertices[4] =
	{
		{ glm::vec3(-128, -128, 50), interp },
		{ glm::vec3(128, -128, 50), bottomRightRgb },
		{ glm::vec3(-128, 128, 50), topLeftRgb },
		{ glm::vec3(128, 128, 50), interp }
	};

	for (auto i = 0; i < 4; ++i)
	{
		std::memcpy(&m_vertices->address()[i], glm::value_ptr(vertices[i].position), 4*3);
		std::memcpy((char *)(&m_vertices->address()[i])+4*3, glm::value_ptr(vertices[i].color), 4*3);
	}

	m_indices->address()[0] = 0;
	m_indices->address()[1] = 1;
	m_indices->address()[2] = 2;
	m_indices->address()[3] = 1;
	m_indices->address()[4] = 3;
	m_indices->address()[5] = 2;

	// load textures
	loadTexture(&m_textures[0].texture, "app0:base.data");
	loadTexture(&m_textures[1].texture, "app0:spec1.data");
	loadTexture(&m_textures[2].texture, "app0:spec2.data");
	loadTexture(&m_textures[3].texture, "app0:spec3.data");
	loadTexture(&m_textures[4].texture, "app0:spec4.data");

	// set movement rates etc
	m_textures[0].position = glm::vec2(256.f, 256.f);
	m_textures[1].position = glm::vec2(256.f, 256.f);
	m_textures[2].position = glm::vec2(256.f, 256.f);
	m_textures[3].position = glm::vec2(256.f, 256.f);
	m_textures[4].position = glm::vec2(256.f, 256.f);

	m_textures[0].dispRate = glm::vec2(0, -16.f);
	m_textures[1].dispRate = glm::vec2(3, -32.f);
	m_textures[2].dispRate = glm::vec2(-31.f, -64.f);
	m_textures[3].dispRate = glm::vec2(12.f, -47.f);
	m_textures[4].dispRate = glm::vec2(22, -72.f);
}

void AnimatedBackground::loadTexture(GxmTexture *texture, const char *file)
{
	auto fp = fopen(file, "rb");
	auto value = malloc(512*512*4);
	fread(value, 1, 512*512*4, fp);
	fclose(fp);

	texture->setSize(512, 512);
	texture->setFormat(GxmTexture::ARGB8);
	texture->allocateStorage();
	texture->setData(value);
	texture->setMinMagFilter(GxmTexture::Linear, GxmTexture::Linear);
	texture->setWrapMode(GxmTexture::Repeat);
	free(value);
}

void AnimatedBackground::update(const Camera *camera, float dt)
{
	for (auto i = 0; i < 5; ++i)
	{
		auto tex = &m_textures[i];
		tex->position = tex->position + tex->dispRate*dt;

		float tileFrequency = 6;
		
		float dxl = tex->position.x/512.f-tileFrequency/2.f;
		float dxu = tex->position.x/512.f+tileFrequency/2.f;
		float dyl = tex->position.y/512.f-tileFrequency/2.f;
		float dyu = tex->position.y/512.f+tileFrequency/2.f;
		
		std::memcpy(&m_texCoords->address()[i], glm::value_ptr(glm::vec2(dxl, dyu)), sizeof(TextureCoord));
		std::memcpy(&m_texCoords->address()[5+i], glm::value_ptr(glm::vec2(dxu, dyu)), sizeof(TextureCoord));
		std::memcpy(&m_texCoords->address()[10+i], glm::value_ptr(glm::vec2(dxl, dyl)), sizeof(TextureCoord));
		std::memcpy(&m_texCoords->address()[15+i], glm::value_ptr(glm::vec2(dxu, dyl)), sizeof(TextureCoord));
	}

	if (m_animateColours)
	{
		m_colourTopLeft.x = std::fmod(m_colourTopLeft.x + dt, 360.f);
		m_colourBottomRight.x = std::fmod(m_colourBottomRight.x + dt, 360.f);

		auto topLeftRgb = glm::rgbColor(m_colourTopLeft);
		auto bottomRightRgb = glm::rgbColor(m_colourBottomRight);

		auto interp = 0.5f*topLeftRgb + 0.5f*bottomRightRgb;

		std::memcpy((char *)(&m_vertices->address()[0])+4*3, glm::value_ptr(interp), 4*3);
		std::memcpy((char *)(&m_vertices->address()[1])+4*3, glm::value_ptr(bottomRightRgb), 4*3);
		std::memcpy((char *)(&m_vertices->address()[2])+4*3, glm::value_ptr(topLeftRgb), 4*3);
		std::memcpy((char *)(&m_vertices->address()[3])+4*3, glm::value_ptr(interp), 4*3);
	}

	m_mvp = camera->projectionMatrix() * camera->viewMatrix() * glm::mat4(1);
}

void AnimatedBackground::draw(SceGxmContext *ctx)
{
	m_program.bind(ctx);
	m_textures[0].texture.bind(ctx, 0);
	m_textures[1].texture.bind(ctx, 1);
	m_textures[2].texture.bind(ctx, 2);
	m_textures[3].texture.bind(ctx, 3);
	m_textures[4].texture.bind(ctx, 4);

	void *uniform = nullptr;
	sceGxmReserveVertexDefaultUniformBuffer(ctx, &uniform);
	m_vertex.setUniformBuffer(uniform);
	m_vertex.setUniformValue(m_mvpIndex, m_mvp);

	sceGxmSetVertexStream(ctx, 0, m_vertices->address());
	sceGxmSetVertexStream(ctx, 1, m_texCoords->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indices->address(), 6);
}

void AnimatedBackground::setColour(glm::vec3 topLeft, glm::vec3 bottomRight)
{
	m_colourTopLeft = glm::hsvColor(topLeft);
	m_colourBottomRight = glm::hsvColor(bottomRight);

	auto interp = 0.5f*topLeft + 0.5f*bottomRight;

	std::memcpy((char *)(&m_vertices->address()[0])+4*3, glm::value_ptr(interp), 4*3);
	std::memcpy((char *)(&m_vertices->address()[1])+4*3, glm::value_ptr(bottomRight), 4*3);
	std::memcpy((char *)(&m_vertices->address()[2])+4*3, glm::value_ptr(topLeft), 4*3);
	std::memcpy((char *)(&m_vertices->address()[3])+4*3, glm::value_ptr(interp), 4*3);
}
