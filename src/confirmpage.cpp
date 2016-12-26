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
	: m_rectangle((960-200), 200, 20)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font12("rsc:/fonts/DroidSans.ttf")
{
	m_font20.setPointSize(20.f);
	m_font12.setPointSize(12.f);
	
	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));

	m_welcomeText.setText("Confirm Page!");
	m_welcomeText.setFont(&m_font20);
	m_welcomeText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press X to continue.");
	m_nextPageDirection.setFont(&m_font12);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	// set models
	setModel(glm::mat4(1));

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
	m_textRenderer.setShaders<ColouredTextVertex>("rsc:/text.vert.cg.gxp", "rsc:/text.frag.cg.gxp");
}

void ConfirmPage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_rectangle.width())/2.f-m_rectangle.radius(), (544-m_rectangle.height())/2.f-m_rectangle.radius(), 0)));

	// welcome text
	m_welcomeText.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_welcomeText.width())/2.f, (544/2.f)+m_welcomeText.height()/2.f+m_nextPageDirection.height()-m_rectangle.radius(), 0)));
	m_nextPageDirection.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_nextPageDirection.width())/2.f, (544/2.f)+m_nextPageDirection.height()/2.f-m_welcomeText.height()-m_rectangle.radius(), 0)));
}

void ConfirmPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_welcomeText);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
}
