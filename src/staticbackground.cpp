/*
 * Enso Installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include "staticbackground.h"
#include "resource.h"

#include <png++/png.hpp>

#include <easyloggingpp/easylogging++.h>

#include <psp2/gxm.h>

namespace
{
	template <typename T>
	struct StaticBackgroundVertex
	{
		using Stream0 = ColouredGeometryVertex;
		using Stream1 = T;

		static std::vector<SceGxmVertexAttribute> attributes(GxmVertexShader *vertexShader)
		{
			std::vector<SceGxmVertexAttribute> attributes(3);
			attributes[0].streamIndex = 0;
			attributes[0].offset = offsetof(Stream0, position);
			attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[0].componentCount = 3;
			attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("position"));

			attributes[1].streamIndex = 0;
			attributes[1].offset = offsetof(Stream0, colour);
			attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[1].componentCount = 4;
			attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("colour"));

			// TODO: texture array?
			attributes[2].streamIndex = 1;
			attributes[2].offset = 0;
			attributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attributes[2].componentCount = 2;
			attributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord"));
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

StaticBackground::StaticBackground(GxmShaderPatcher *patcher)
	: m_renderer(patcher)
	, m_texCoords(4, SCE_GXM_MEMORY_ATTRIB_READ)
{
	// set our shader program
	m_renderer.setShaders<StaticBackgroundVertex<glm::vec2>>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");
	m_rectangle.setFragmentTask(std::bind(&StaticBackground::fragmentTask, this, std::placeholders::_1));

	m_rectangle.setWidth(1920);
	m_rectangle.setHeight(544);
	//m_rectangle.setTranslation(-1920*2, -1920*2, -256);
	m_rectangle.setTranslation(0, 0, -1);

	auto bottomRightRgb = glm::vec4(1.f, 1.f, 1.f, 1.f);
	auto topLeftRgb = glm::vec4(1.f, 1.f, 1.f, 1.f);
	setColour(topLeftRgb, bottomRightRgb);

	// load textures
	loadTexture(&m_texture, "textures/ensobg.png");

	// set our texCoords
	m_texCoords.address()[0] = glm::vec2(0, 1);
	m_texCoords.address()[1] = glm::vec2(1, 1);
	m_texCoords.address()[2] = glm::vec2(0, 0);
	m_texCoords.address()[3] = glm::vec2(1, 0);
}

void StaticBackground::update(float dt)
{

}

void StaticBackground::draw(SceGxmContext *ctx, const Camera *camera)
{
	m_renderer.draw(ctx, camera, &m_rectangle);
}

void StaticBackground::loadTexture(GxmTexture *texture, const char *file)
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
	texture->setFormat(GxmTexture::ABGR8);
	texture->allocateStorage();
	texture->setData(image.get_pixbuf().get_bytes().data());
	texture->setMinMagFilter(GxmTexture::Linear, GxmTexture::Linear);
	texture->setWrapMode(GxmTexture::Repeat);
}

void StaticBackground::fragmentTask(SceGxmContext *ctx)
{
	m_texture.bind(ctx, 0);
	sceGxmSetVertexStream(ctx, 1, m_texCoords.address());
}

void StaticBackground::setColour(glm::vec4 topLeft, glm::vec4 bottomRight)
{
	auto interp = 0.5f*topLeft + 0.5f*bottomRight;
	m_topLeftColour = topLeft;
	m_bottomRightColour = bottomRight;
	m_rectangle.setBottomLeftColour(interp);
	m_rectangle.setBottomRightColour(bottomRight);
	m_rectangle.setTopLeftColour(topLeft);
	m_rectangle.setTopRightColour(interp);
}
