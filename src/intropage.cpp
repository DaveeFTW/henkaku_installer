/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include "intropage.h"
#include "focusinevent.h"
#include "easingcurves.h"
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

IntroPage::IntroPage(GxmShaderPatcher *patcher)
	: m_renderer(patcher)
	, m_rectangle(960, 544)
	, m_ensoRenderer(patcher)
	, m_texCoords(4, SCE_GXM_MEMORY_ATTRIB_READ)
{
	m_rectangle.setColour(glm::vec4(14.f/256.f, 14.f/256.f, 14.f/256.f, 1.f));

	SceGxmBlendInfo blendInfo;
	blendInfo.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.colorMask = SCE_GXM_COLOR_MASK_ALL;

	m_renderer.setBlendInfo(&blendInfo);
	m_renderer.setShaders<ColouredGeometryVertex>("rsc:/colour.vert.cg.gxp", "rsc:/colour.frag.cg.gxp");

	// set our shader program
	m_ensoRenderer.setBlendInfo(&blendInfo);
	m_ensoRenderer.setShaders<StaticBackgroundVertex<glm::vec2>>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");
	m_ensoRectangle.setFragmentTask(std::bind(&IntroPage::fragmentTask, this, std::placeholders::_1));

	auto white = glm::vec4(1.f, 1.f, 1.f, 1.f);
	m_ensoRectangle.setColour(white);

	// load textures
	loadTexture(&m_texture, "textures/enso.png");

	// set our texCoords
	m_texCoords.address()[0] = glm::vec2(0, 1);
	m_texCoords.address()[1] = glm::vec2(1, 1);
	m_texCoords.address()[2] = glm::vec2(0, 0);
	m_texCoords.address()[3] = glm::vec2(1, 0);

	m_ensoRectangle.setWidth(m_texture.width());
	m_ensoRectangle.setHeight(m_texture.height());

	positionComponents();

	// fade in text
	m_fadeIn.setStart(0.f);
	m_fadeIn.setEnd(1.f);
	m_fadeIn.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeIn.setStepHandler([this](float step)
	{
		this->m_rectangle.setColour(glm::vec4(14.f/256.f, 14.f/256.f, 14.f/256.f, step));
	});

	m_fadeIn.setDuration(1500);
	m_fadeIn.setCompletionHandler([this]()
	{
		this->m_pauseAnimation.start();
	});

	// pause duration
	this->m_pauseAnimation.setStart(0.f);
	this->m_pauseAnimation.setEnd(0.f);
	this->m_pauseAnimation.setDuration(3000);
	this->m_pauseAnimation.setCompletionHandler([this]()
	{
		this->m_fadeOut.start();
	});

	// fade out text
	m_fadeOut.setStart(0.f);
	m_fadeOut.setEnd(1.f);
	m_fadeOut.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeOut.setStepHandler([this](float step)
	{
		this->m_rectangle.setColour(glm::vec4(14.f/256.f, 14.f/256.f, 14.f/256.f, 1.0-step));
	});

	m_fadeOut.setDuration(500);
	m_fadeOut.setCompletionHandler([this]()
	{
		if (this->m_exitCallback)
			this->m_exitCallback();
	});

	// fade out the background
	m_fadeOutBg.setStart(0.f);
	m_fadeOutBg.setEnd(1.f);
	m_fadeOutBg.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeOutBg.setStepHandler([this](float step)
	{
		this->m_rectangle.setColour(glm::vec4(14.f/256.f, 14.f/256.f, 14.f/256.f, 1.0-step));
	});

	m_fadeOutBg.setDuration(2000);

	m_fadeOutBg.setCompletionHandler([this]()
	{
		m_fadeOutEnso.start();
	});

	// fade out the enso
	m_fadeOutEnso.setStart(0.f);
	m_fadeOutEnso.setEnd(1.f);
	m_fadeOutEnso.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeOutEnso.setStepHandler([this](float step)
	{
		this->m_ensoRectangle.setColour(glm::vec4(1.f, 1.f, 1.f, 1.0-step));
	});

	m_fadeOutEnso.setDuration(2000);

	m_fadeOutEnso.setCompletionHandler([this]()
	{
		if (this->m_exitCallback)
			this->m_exitCallback();
	});
}

void IntroPage::update(float dt)
{
	if (!m_fadeIn.complete())
		m_fadeIn.update(dt);

	if (!m_pauseAnimation.complete())
		m_pauseAnimation.update(dt);

	if (!m_fadeOut.complete())
		m_fadeOut.update(dt);

	if  (!m_fadeOutBg.complete())
		m_fadeOutBg.update(dt);

	if  (!m_fadeOutEnso.complete())
		m_fadeOutEnso.update(dt);
}

void IntroPage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_ensoRectangle.setWorldMatrix(model);
}

void IntroPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_ensoRenderer.draw(ctx, camera, &m_ensoRectangle);
}

void IntroPage::setExitTrigger(std::function<void ()> callback)
{
	m_exitCallback = callback;
}

void IntroPage::positionComponents(void)
{
	// set world matrices
	m_rectangle.setWorldMatrix(modelMatrix());
	m_ensoRectangle.setWorldMatrix(modelMatrix());

	m_rectangle.setTranslation(glm::vec3(1, 0, 0)); // TODO: investigate an "off-by-one" error
	m_ensoRectangle.setTranslation(glm::vec3(1, 0, 0)); // TODO: investigate an "off-by-one" error
}

void IntroPage::loadTexture(GxmTexture *texture, const char *file)
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

void IntroPage::fragmentTask(SceGxmContext *ctx)
{
	m_texture.bind(ctx, 0);
	sceGxmSetVertexStream(ctx, 1, m_texCoords.address());
}

void IntroPage::onEvent(Event *event)
{
	if (event->type() == Event::FocusIn)
	{
		m_fadeOutBg.start();
	}
}

