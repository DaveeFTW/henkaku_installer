/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include "warningpage.h"
#include "easingcurves.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

WarningPage::WarningPage(GxmShaderPatcher *patcher)
	: m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
	, m_transitionGuard(true)
{
	m_font20.setPointSize(20.f);
	m_font12.setPointSize(10.f);
	m_font8.setPointSize(8.f);

	m_titleText.setText("Please Read");
	m_titleText.setFont(&m_font20);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	// TODO: manually word wrap because lazy
	m_description.setText("This software will make PERMANENT modifications");
	m_description.setFont(&m_font12);
	m_description.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_description2.setText("to your Vita. If anything goes wrong, there is NO");
	m_description2.setFont(&m_font12);
	m_description2.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_description3.setText("RECOVERY (not even with a hardware flasher). The");
	m_description3.setFont(&m_font12);
	m_description3.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_description4.setText("creators provide this tool \"as is\", without warranty");
	m_description4.setFont(&m_font12);
	m_description4.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_description5.setText("of any kind, express or implied and cannot be held");
	m_description5.setFont(&m_font12);
	m_description5.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_description6.setText("liable for any damage done.");
	m_description6.setFont(&m_font12);
	m_description6.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_nextPageDirection.setText("Press CIRCLE to accept these terms otherwise press any other key to exit.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	// position our UI
	positionComponents();

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

	m_fadeInTitle.setStart(0.f);
	m_fadeInTitle.setEnd(1.f);
	m_fadeInTitle.setDuration(750);

	m_fadeInTitle.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeInTitle.setStepHandler([this](float step)
	{
		this->m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, step));
	});

	m_fadeInTitle.setCompletionHandler([this]()
	{
		m_fadeInText.start();
	});

	m_fadeInText.setStart(0.f);
	m_fadeInText.setEnd(1.f);
	m_fadeInText.setDuration(1750);

	m_fadeInText.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeInText.setStepHandler([this](float step)
	{
		this->m_description.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_description2.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_description3.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_description4.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_description5.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_description6.setColour(glm::vec4(1.f, 1.f, 1.f, step));
	});

	m_fadeInText.setCompletionHandler([this]()
	{
		m_fadeInDirection.start();
	});

	// direction text
	m_fadeInDirection.setStart(0.f);
	m_fadeInDirection.setEnd(1.f);
	m_fadeInDirection.setDuration(1750);

	m_fadeInDirection.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeInDirection.setStepHandler([this](float step)
	{
		this->m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, step));
	});

	m_fadeInDirection.setCompletionHandler([this]()
	{
		m_transitionGuard = true;
	});
}

void WarningPage::onModelChanged(glm::mat4 model)
{
	m_titleText.setWorldMatrix(model);
	m_description.setWorldMatrix(model);
	m_description2.setWorldMatrix(model);
	m_description3.setWorldMatrix(model);
	m_description4.setWorldMatrix(model);
	m_description5.setWorldMatrix(model);
	m_description6.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);
}

void WarningPage::positionComponents(void)
{
	constexpr auto seperationPadding = 544.f * (2.f/100.f);
	auto boxOffsetY = (544.f + 400.f)/2.f;

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setTranslation((960-m_titleText.width())/2.f, boxOffsetY);

	boxOffsetY -= seperationPadding*3 + m_description.height();
	m_description.setTranslation((960-m_description.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description2.height()+seperationPadding;
	m_description2.setTranslation((960-m_description2.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description3.height()+seperationPadding;
	m_description3.setTranslation((960-m_description3.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description3.height()+seperationPadding;
	m_description4.setTranslation((960-m_description3.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description3.height()+seperationPadding;
	m_description5.setTranslation((960-m_description3.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description3.height()+seperationPadding;
	m_description6.setTranslation((960-m_description3.width())/2.f, boxOffsetY);

	// centre within bottom box
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - 400.f)/2.f);
}

void WarningPage::update(float dt)
{
	if (!m_fadeInTitle.complete())
		m_fadeInTitle.update(dt);

	if (!m_fadeInText.complete())
		m_fadeInText.update(dt);

	if (!m_fadeInDirection.complete())
		m_fadeInDirection.update(dt);
}

void WarningPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_description);
	m_textRenderer.draw(ctx, camera, &m_description2);
	m_textRenderer.draw(ctx, camera, &m_description3);
	m_textRenderer.draw(ctx, camera, &m_description4);
	m_textRenderer.draw(ctx, camera, &m_description5);
	m_textRenderer.draw(ctx, camera, &m_description6);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
}

void WarningPage::onEvent(Event *event)
{
	if (event->type() == Event::FocusIn)
	{
		m_transitionGuard = false;
		m_fadeInTitle.start();
	}
}

void WarningPage::setExitTrigger(std::function<void ()> callback)
{
	m_exitCallback = callback;
}

bool WarningPage::transitionGuard() const
{
	return m_transitionGuard;
}
