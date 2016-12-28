/*
 * configpage.h - let the user decide which features they would like enabled or disabled
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "configpage.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

ConfigPage::ConfigPage(GxmShaderPatcher *patcher)
	: m_rectangle((960-200), 400, 20)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font10("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
	, m_menu(&m_renderer, &m_textRenderer)
	, m_unsafeCheckbox(&m_renderer, &m_textRenderer)
	, m_spoofCheckbox(&m_renderer, &m_textRenderer)
{
	m_font20.setPointSize(20.f);
	m_font10.setPointSize(10.f);
	m_font8.setPointSize(8.f);
	
	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));

	m_titleText.setText("Configuration");
	m_titleText.setFont(&m_font20);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press right to continue.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_unsafeLabel.setText("Enable support for plugins and unsafe homebrew");
	m_unsafeLabel.setFont(&m_font10);
	m_unsafeLabel.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_unsafeLabelDesc.setText("Unstable or malicious content can permanently damage your Vita!");
	m_unsafeLabelDesc.setFont(&m_font8);
	m_unsafeLabelDesc.setColour(glm::vec4(0xf7/255.f, 0xb6/255.f, 0x54/255.f, 1.f));

	m_unsafeCheckbox.setText(&m_unsafeLabel);

	m_spoofLabel.setText("Enable version spoofing to block update prompts");
	m_spoofLabel.setFont(&m_font10);
	m_spoofLabel.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_spoofLabelDesc.setText("TODO:");
	m_spoofLabelDesc.setFont(&m_font8);
	m_spoofLabelDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_spoofCheckbox.setText(&m_spoofLabel);

	m_menu.add({&m_unsafeCheckbox, &m_unsafeLabelDesc});
	m_menu.add({&m_spoofCheckbox, &m_spoofLabelDesc});

	m_menu.setSelectionWidth(m_rectangle.width() + m_rectangle.radius()*2.f, -m_rectangle.radius());

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

	m_renderer.setBlendInfo(&blendInfo);
	m_renderer.setShaders<ColouredGeometryVertex>("rsc:/colour.vert.cg.gxp", "rsc:/colour.frag.cg.gxp");

	m_textRenderer.setBlendInfo(&blendInfo);
	m_textRenderer.setShaders<ColouredTextureVertex>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");
}

void ConfigPage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_titleText.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);
	m_menu.setWorldMatrix(model);
}

void ConfigPage::positionComponents(void)
{
	constexpr auto seperationPadding = 544.f * (2.f/100.f);
	auto boxOffsetX = (960.f - m_rectangle.width())/2.f;
	auto boxOffsetY = (544.f + m_rectangle.height())/2.f;

	// centre within view
	m_rectangle.setTranslation((960-m_rectangle.width())/2.f-m_rectangle.radius(), (544-m_rectangle.height())/2.f-m_rectangle.radius());

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setTranslation((960-m_titleText.width())/2.f, boxOffsetY);

	auto ytop = boxOffsetY-seperationPadding;
	auto ybottom = (544.f - m_rectangle.height())/2.f+m_nextPageDirection.height()+seperationPadding;
	auto height = ytop - ybottom;

	m_menu.setTranslation(boxOffsetX, boxOffsetY-height/2.f+m_menu.height()/2.f);
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - m_rectangle.height())/2.f);
}

void ConfigPage::update(float dt)
{
	m_menu.update(dt);
}

void ConfigPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
	m_menu.draw(ctx, camera);
}

void ConfigPage::onEvent(ButtonEvent *event)
{
	if (event->buttons() == SCE_CTRL_UP)
	{
		m_menu.up();
	}
	else if (event->buttons() == SCE_CTRL_DOWN)
	{
		m_menu.down();
	}
	else if (event->buttons() == SCE_CTRL_CROSS)
	{
		m_menu.toggle();
	}
}

ConfigPage::UnsafeHomebrew ConfigPage::unsafeHomebrew(void) const
{
	return m_unsafeCheckbox.checked() ? UnsafeHomebrew::Enabled : UnsafeHomebrew::Disabled;
}

ConfigPage::VersionSpoofing ConfigPage::versionSpoofing(void) const
{
	return m_spoofCheckbox.checked() ? VersionSpoofing::Enabled : VersionSpoofing::Disabled;
}
