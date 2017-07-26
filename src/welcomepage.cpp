/*
 * welcomepage.cpp - the first page the user is presented
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "welcomepage.h"
#include "focusinevent.h"
#include "easingcurves.h"

WelcomePage::WelcomePage(GxmShaderPatcher *patcher)
	: m_textRenderer(patcher)
	, m_font28("rsc:/fonts/DroidSans.ttf")
{
	m_font28.setPointSize(28.f);

	m_welcomeText.setText("Welcome");
	m_welcomeText.setFont(&m_font28);
	m_welcomeText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	SceGxmBlendInfo blendInfo;
	blendInfo.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.colorMask = SCE_GXM_COLOR_MASK_ALL;

	m_textRenderer.setBlendInfo(&blendInfo);
	m_textRenderer.setShaders<ColouredTextureVertex>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");

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
		this->m_welcomeText.setColour(glm::vec4(1.0, 1.0, 1.0, step));
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
		this->m_welcomeText.setColour(glm::vec4(1.0, 1.0, 1.0, 1.0-step));
	});

	m_fadeOut.setDuration(500);
	m_fadeOut.setCompletionHandler([this]()
	{
		if (this->m_exitCallback)
			this->m_exitCallback();
	});
}

void WelcomePage::update(float dt)
{
	if (!m_fadeIn.complete())
		m_fadeIn.update(dt);

	if (!m_pauseAnimation.complete())
		m_pauseAnimation.update(dt);

	if (!m_fadeOut.complete())
		m_fadeOut.update(dt);
}

void WelcomePage::onModelChanged(glm::mat4 model)
{
	m_welcomeText.setWorldMatrix(model);
}

void WelcomePage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_textRenderer.draw(ctx, camera, &m_welcomeText);
}

void WelcomePage::setExitTrigger(std::function<void ()> callback)
{
	m_exitCallback = callback;
}

void WelcomePage::positionComponents(void)
{
	// set world matrices
	m_welcomeText.setWorldMatrix(modelMatrix());
	m_welcomeText.setTranslation(glm::vec3((960-m_welcomeText.width())/2.f, (544/2.f)-m_welcomeText.height()/3.f, 0));
}

void WelcomePage::onEvent(Event *event)
{
	if (event->type() == Event::FocusIn)
	{
		m_fadeIn.start();
	}
}
