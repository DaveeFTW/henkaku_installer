/*
 * failurepage.cpp - display installation failure information
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "failurepage.h"
#include "focusinevent.h"
#include "easingcurves.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

FailurePage::FailurePage(GxmShaderPatcher *patcher)
	: m_rectangle(960, 544)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
{
	m_font20.setPointSize(20.f);
	m_font12.setPointSize(10.f);
	m_font8.setPointSize(8.f);

	m_titleText.setText("An error has occured...");
	m_titleText.setFont(&m_font20);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	setMessage("Unknown error. Please consult assistance online.");

	m_rectangle.setColour(glm::vec4(1.f, 0.f, 0.f, 0.0f));

	m_nextPageDirection.setText("Press any button to exit.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	positionComponents();

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

	m_textRenderer.setBlendInfo(&blendInfo);
	m_textRenderer.setShaders<ColouredTextureVertex>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");

	// animations
	// fade in red
	m_fadeInError.setStart(0.f);
	m_fadeInError.setEnd(0.2f);
	m_fadeInError.setDuration(750);
	m_fadeInError.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeInError.setStepHandler([this](float step)
	{
		this->m_rectangle.setColour(glm::vec4(1.0f, 0.f, 0.f, step));
	});

	m_fadeInError.setCompletionHandler([this]()
	{
		m_fadeInTitle.start();
	});

	// fade in title
	m_fadeInTitle.setStart(0.f);
	m_fadeInTitle.setEnd(1.0f);
	m_fadeInTitle.setDuration(750);
	m_fadeInTitle.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeInTitle.setStepHandler([this](float step)
	{
		this->m_titleText.setColour(glm::vec4(1.0f, 1.f, 1.f, step));
	});

	m_fadeInTitle.setCompletionHandler([this]()
	{
		m_fadeInMessage.start();
	});

	// fade in message
	m_fadeInMessage.setStart(0.f);
	m_fadeInMessage.setEnd(1.0f);
	m_fadeInMessage.setDuration(750);
	m_fadeInMessage.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeInMessage.setStepHandler([this](float step)
	{
		for (auto &msg : this->m_message)
			msg.setColour(glm::vec4(1.0f, 1.f, 1.f, step));
	});

	m_fadeInMessage.setCompletionHandler([this]()
	{
		m_fadeInDirection.start();
	});

	// fade in direction
	m_fadeInDirection.setStart(0.f);
	m_fadeInDirection.setEnd(1.0f);
	m_fadeInDirection.setDuration(750);
	m_fadeInDirection.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_fadeInDirection.setStepHandler([this](float step)
	{
		this->m_nextPageDirection.setColour(glm::vec4(1.0f, 1.f, 1.f, step));
	});

	m_fadeInDirection.setCompletionHandler([this]()
	{
	});
}

void FailurePage::update(float dt)
{
	if (!m_fadeInError.complete())
		m_fadeInError.update(dt);

	if (!m_fadeInTitle.complete())
		m_fadeInTitle.update(dt);

	if (!m_fadeInMessage.complete())
		m_fadeInMessage.update(dt);

	if (!m_fadeInDirection.complete())
		m_fadeInDirection.update(dt);
}

void FailurePage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_titleText.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);

	for (auto &msg : m_message)
		msg.setWorldMatrix(model);
}

void FailurePage::positionComponents()
{
	m_rectangle.setWorldMatrix(modelMatrix());
	m_titleText.setWorldMatrix(modelMatrix());
	m_nextPageDirection.setWorldMatrix(modelMatrix());

	for (auto &msg : m_message)
		msg.setWorldMatrix(modelMatrix());

	constexpr auto seperationPadding = 544.f * (2.f/100.f);
	auto boxOffsetY = (544.f + 400.f)/2.f;

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setTranslation((960-m_titleText.width())/2.f, boxOffsetY);

	boxOffsetY -= seperationPadding*4;

	for (auto &msg : m_message)
	{
		boxOffsetY -= msg.height()+seperationPadding;
		msg.setTranslation((960-msg.width())/2.f, boxOffsetY);
	}

	// centre within bottom box
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - 400.f)/2.f);
}

void FailurePage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_titleText);

	for (auto &msg : m_message)
		m_textRenderer.draw(ctx, camera, &msg);

	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
}

void FailurePage::onEvent(Event *event)
{
	if (event->type() == Event::FocusIn)
	{
		m_fadeInError.start();
	}
}

void FailurePage::setMessage(const std::string &msg)
{
	m_message.clear();

	Text message;

	message.setText(msg);
	message.setFont(&m_font12);
	message.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_message.push_back(std::move(message));
	positionComponents();
}

void FailurePage::setMessage(std::initializer_list<std::string> msg)
{
	m_message.clear();

	for (auto &line : msg)
	{
		Text message;

		message.setText(line);
		message.setFont(&m_font12);
		message.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

		m_message.push_back(std::move(message));
	}

	positionComponents();
}

void FailurePage::setMessage(const std::vector<std::string>& msg)
{
	m_message.clear();

	for (auto &line : msg)
	{
		Text message;

		message.setText(line);
		message.setFont(&m_font12);
		message.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

		m_message.push_back(std::move(message));
	}

	positionComponents();
}
