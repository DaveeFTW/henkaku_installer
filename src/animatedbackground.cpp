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
#include "resource.h"
#include "shaderutility.h"
#include "rectangle.h"
#include "geometryrenderer.h"
#include "vertextypes.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/transform.hpp>

#include <png++/png.hpp>

#include <easyloggingpp/easylogging++.h>

#include <psp2/gxm.h>

namespace
{
	template <typename T>
	struct AnimatedBackgroundVertex
	{
		using Stream0 = ColouredGeometryVertex;
		using Stream1 = T;

		static std::vector<SceGxmVertexAttribute> attributes(GxmVertexShader *vertexShader)
		{
			std::vector<SceGxmVertexAttribute> attributes(7);
			attributes[0].streamIndex = 0;
			attributes[0].offset = offsetof(Stream0, position);
			attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[0].componentCount = 3;
			attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("position"));

			attributes[1].streamIndex = 0;
			attributes[1].offset = offsetof(Stream0, colour);
			attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[1].componentCount = 3;
			attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("colour"));

			// TODO: texture array?
			attributes[2].streamIndex = 1;
			attributes[2].offset = offsetof(Stream1, texCoord[0]);
			attributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[2].componentCount = 2;
			attributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord1"));

			attributes[3].streamIndex = 1;
			attributes[3].offset = offsetof(Stream1, texCoord[1]);
			attributes[3].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[3].componentCount = 2;
			attributes[3].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord2"));

			attributes[4].streamIndex = 1;
			attributes[4].offset = offsetof(Stream1, texCoord[2]);
			attributes[4].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[4].componentCount = 2;
			attributes[4].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord3"));

			attributes[5].streamIndex = 1;
			attributes[5].offset = offsetof(Stream1, texCoord[3]);
			attributes[5].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[5].componentCount = 2;
			attributes[5].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord4"));

			attributes[6].streamIndex = 1;
			attributes[6].offset = offsetof(Stream1, texCoord[4]);
			attributes[6].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[6].componentCount = 2;
			attributes[6].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord5"));
			return attributes;
		}

		static std::vector<SceGxmVertexStream> streams(void)
		{
			std::vector<SceGxmVertexStream> streams(2);
			streams[0].stride = sizeof(Stream0);
			streams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
			streams[1].stride = sizeof(Stream1);
			streams[1].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
			return streams;
		}

		constexpr static int streamCount(void)
		{
			return 2;
		}
	};
} // anonymous namespace

AnimatedBackground::AnimatedBackground(GxmShaderPatcher *patcher)
	: m_renderer(patcher)
	, m_texCoords(std::make_unique<GpuMemoryBlock<TextureCoordVertex>>
	(
		4,
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_animateColours(false)
{
	// set our shader program
	m_renderer.setShaders<AnimatedBackgroundVertex<TextureCoordVertex>>("rsc:/animbg.vert.cg.gxp", "rsc:/animbg.frag.cg.gxp");
	m_rectangle.setFragmentTask(std::bind(&AnimatedBackground::fragmentTask, this, std::placeholders::_1));

	auto bottomRightRgb = glm::vec3(172.f/255.f, 228.f/255.f, 234.f/255.f);
	auto topLeftRgb = glm::vec3(255.f/255.f, 228.f/255.f, 234.f/255.f);
	
	m_colourTopLeft = glm::hsvColor(topLeftRgb);
	m_colourBottomRight = glm::hsvColor(bottomRightRgb);

	auto interp = 0.5f*topLeftRgb + 0.5f*bottomRightRgb;
	
	ColouredGeometryVertex vertices[4] =
	{
		{ glm::vec3(-2048, -2048, -256), interp },
		{ glm::vec3(2048, -2048, -256), bottomRightRgb },
		{ glm::vec3(-2048, 2048, -256), topLeftRgb },
		{ glm::vec3(2048, 2048, -256), interp }
	};

	m_rectangle.setBottomLeft(vertices[0]);
	m_rectangle.setBottomRight(vertices[1]);
	m_rectangle.setTopLeft(vertices[2]);
	m_rectangle.setTopRight(vertices[3]);

	// load textures
	loadTexture(&m_textures[0].texture, "textures/bgbase.png");
	loadTexture(&m_textures[1].texture, "textures/bgspec1.png");
	loadTexture(&m_textures[2].texture, "textures/bgspec2.png");
	loadTexture(&m_textures[3].texture, "textures/bgspec3.png");
	loadTexture(&m_textures[4].texture, "textures/bgspec4.png");

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
	class VectorStream : public std::basic_streambuf<char, std::char_traits<char>>
	{
	public:
		VectorStream(std::vector<char>& v)
		{
			setg(v.data(), v.data(), v.data()+v.size());
		}
	};
	
	std::string prefix = TEXTURE_SOURCE_PREFIX;
	auto fileData = Resource::read(prefix + file);
	
	if (!fileData.good)
	{
		LOG(FATAL) << "could not load texture: " << prefix << file;
	}

	VectorStream vstream(fileData.data);
	std::istream stream(&vstream);
	
	png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> image(stream);
	
	texture->setSize(image.get_width(), image.get_height());
	texture->setFormat(GxmTexture::ARGB8);
	texture->allocateStorage();
	texture->setData(image.get_pixbuf().get_bytes().data());
	texture->setMinMagFilter(GxmTexture::Linear, GxmTexture::Linear);
	texture->setWrapMode(GxmTexture::Repeat);
}

void AnimatedBackground::fragmentTask(SceGxmContext *ctx)
{
	m_textures[0].texture.bind(ctx, 0);
	m_textures[1].texture.bind(ctx, 1);
	m_textures[2].texture.bind(ctx, 2);
	m_textures[3].texture.bind(ctx, 3);
	m_textures[4].texture.bind(ctx, 4);
	
	sceGxmSetVertexStream(ctx, 1, m_texCoords->address());
}

void AnimatedBackground::update(const Camera *camera, float dt)
{
	for (auto i = 0; i < 5; ++i)
	{
		auto tex = &m_textures[i];
		tex->position = tex->position + tex->dispRate*dt;

		float tileFrequency = 3;
		
		float dxl = tex->position.x/512.f-tileFrequency/2.f;
		float dxu = tex->position.x/512.f+tileFrequency/2.f;
		float dyl = tex->position.y/512.f-tileFrequency/2.f;
		float dyu = tex->position.y/512.f+tileFrequency/2.f;

		m_texCoords->address()[0].texCoord[i] = glm::vec2(dxl, dyu);
		m_texCoords->address()[1].texCoord[i] = glm::vec2(dxu, dyu);
		m_texCoords->address()[2].texCoord[i] = glm::vec2(dxl, dyl);
		m_texCoords->address()[3].texCoord[i] = glm::vec2(dxu, dyl);
	}

/*	if (m_animateColours)
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
	}*/
}

void AnimatedBackground::draw(SceGxmContext *ctx, const Camera *camera)
{
	m_renderer.draw(ctx, camera, &m_rectangle);
}

void AnimatedBackground::setColour(glm::vec3 topLeft, glm::vec3 bottomRight)
{
	/*m_colourTopLeft = glm::hsvColor(topLeft);
	m_colourBottomRight = glm::hsvColor(bottomRight);

	auto interp = 0.5f*topLeft + 0.5f*bottomRight;

	std::memcpy((char *)(&m_vertices->address()[0])+4*3, glm::value_ptr(interp), 4*3);
	std::memcpy((char *)(&m_vertices->address()[1])+4*3, glm::value_ptr(bottomRight), 4*3);
	std::memcpy((char *)(&m_vertices->address()[2])+4*3, glm::value_ptr(topLeft), 4*3);
	std::memcpy((char *)(&m_vertices->address()[3])+4*3, glm::value_ptr(interp), 4*3);*/
}
