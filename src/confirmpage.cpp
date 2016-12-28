/*
 * confirmpage.cpp - visualise the user's selected options before installation
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "confirmpage.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

ConfirmPage::ConfirmPage(GxmShaderPatcher *patcher)
	: m_rectangle((960-200), 400, 20)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
{
	m_font20.setPointSize(20.f);
	m_font12.setPointSize(10.f);
	m_font8.setPointSize(8.f);
	
	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));

	m_titleText.setText("Confirm Installation");
	m_titleText.setFont(&m_font20);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	// TODO: manually word wrap because lazy
	m_description.setText("You are installing HENkaku with the following options:");
	m_description.setFont(&m_font12);
	m_description.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_resetText.setText("Reset all HENkaku and taiHEN data");
	m_resetText.setFont(&m_font12);
	m_resetText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_unsafeText.setText("Enable plugins and unsafe homebrew");
	m_unsafeText.setFont(&m_font12);
	m_unsafeText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_spoofText.setText("Enable version spoofing to prevent updates");
	m_spoofText.setFont(&m_font12);
	m_spoofText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_offlineText.setText("Install HENkaku offline enabler");
	m_offlineText.setFont(&m_font12);
	m_offlineText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press right to continue.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

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

void ConfirmPage::setConfigurationOptions(InstallerView::HenkakuOptions options)
{
	m_resetDecisionText.setText(options.resetAll ? "yes" : "no");
	m_resetDecisionText.setFont(&m_font12);
	m_resetDecisionText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_unsafeDecisionText.setText(options.unsafeHomebrew ? "yes" : "no");
	m_unsafeDecisionText.setFont(&m_font12);
	m_unsafeDecisionText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_spoofDecisionText.setText(options.versionSpoofing ? "yes" : "no");
	m_spoofDecisionText.setFont(&m_font12);
	m_spoofDecisionText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_offlineDecisionText.setText(options.offlineInstaller ? "yes" : "no");
	m_offlineDecisionText.setFont(&m_font12);
	m_offlineDecisionText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	positionComponents();
}

void ConfirmPage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_titleText.setWorldMatrix(model);
	m_description.setWorldMatrix(model);
	m_resetText.setWorldMatrix(model);
	m_unsafeText.setWorldMatrix(model);
	m_spoofText.setWorldMatrix(model);
	m_offlineText.setWorldMatrix(model);
	m_resetDecisionText.setWorldMatrix(model);
	m_unsafeDecisionText.setWorldMatrix(model);
	m_spoofDecisionText.setWorldMatrix(model);
	m_offlineDecisionText.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);
}

void ConfirmPage::positionComponents(void)
{
	constexpr auto seperationPadding = 544.f * (2.f/100.f);
	auto boxOffsetX = (960.f - m_rectangle.width())/2.f;
	auto boxOffsetY = (544.f + m_rectangle.height())/2.f;

	// centre within view
	m_rectangle.setTranslation((960-m_rectangle.width())/2.f-m_rectangle.radius(), (544-m_rectangle.height())/2.f-m_rectangle.radius());

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setTranslation((960-m_titleText.width())/2.f, boxOffsetY);

	boxOffsetY -= seperationPadding*3 + m_description.height();
	m_description.setTranslation((960-m_description.width())/2.f, boxOffsetY);

	boxOffsetY -= m_resetText.height()+seperationPadding*5;
	m_resetText.setTranslation(boxOffsetX, boxOffsetY);
	m_resetDecisionText.setTranslation(960.f-boxOffsetX-m_resetDecisionText.width(), boxOffsetY);

	boxOffsetY -= m_unsafeText.height()+seperationPadding;
	m_unsafeText.setTranslation(boxOffsetX, boxOffsetY);
	m_unsafeDecisionText.setTranslation(960.f-boxOffsetX-m_unsafeDecisionText.width(), boxOffsetY);

	boxOffsetY -= m_spoofText.height()+seperationPadding;
	m_spoofText.setTranslation(boxOffsetX, boxOffsetY);
	m_spoofDecisionText.setTranslation(960.f-boxOffsetX-m_spoofDecisionText.width(), boxOffsetY);

	boxOffsetY -= m_offlineText.height()+seperationPadding;
	m_offlineText.setTranslation(boxOffsetX, boxOffsetY);
	m_offlineDecisionText.setTranslation(960.f-boxOffsetX-m_offlineDecisionText.width(), boxOffsetY);

	// centre within bottom box
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - m_rectangle.height())/2.f);
}

void ConfirmPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_description);
	m_textRenderer.draw(ctx, camera, &m_resetText);
	m_textRenderer.draw(ctx, camera, &m_unsafeText);
	m_textRenderer.draw(ctx, camera, &m_spoofText);
	m_textRenderer.draw(ctx, camera, &m_offlineText);
	m_textRenderer.draw(ctx, camera, &m_resetDecisionText);
	m_textRenderer.draw(ctx, camera, &m_unsafeDecisionText);
	m_textRenderer.draw(ctx, camera, &m_spoofDecisionText);
	m_textRenderer.draw(ctx, camera, &m_offlineDecisionText);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
}
