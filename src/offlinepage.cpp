/*
 * offlinepage.cpp - let the user decide if they wish to use the henkaku offline enable
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "offlinepage.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

OfflinePage::OfflinePage(GxmShaderPatcher *patcher)
	: m_rectangle((960-200), 400, 20)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font20("rsc:/fonts/DroidSans.ttf")
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
	, m_checkbox(&m_renderer, &m_textRenderer)
{
	m_font20.setPointSize(20.f);
	m_font12.setPointSize(10.f);
	m_font8.setPointSize(8.f);
	
	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));

	m_titleText.setText("Offline HENkaku");
	m_titleText.setFont(&m_font20);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	// TODO: manually word wrap because lazy
	m_description.setText("Would you like to use the HENkaku offline enabler");
	m_description.setFont(&m_font12);
	m_description.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_description2.setText("from the Email application? Check out");
	m_description2.setFont(&m_font12);
	m_description2.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_description3.setText("https://henkaku.xyz/usage for details");
	m_description3.setFont(&m_font12);
	m_description3.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press right to continue.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_checkBoxLabel.setText("Yes, install offline HENkaku");
	m_checkBoxLabel.setFont(&m_font12);
	m_checkBoxLabel.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_checkbox.setText(&m_checkBoxLabel);
	m_checkbox.setChecked(true);

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

void OfflinePage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_titleText.setWorldMatrix(model);
	m_description.setWorldMatrix(model);
	m_description2.setWorldMatrix(model);
	m_description3.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);
	m_checkbox.setWorldMatrix(model);
}

void OfflinePage::positionComponents(void)
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

	boxOffsetY -= m_description2.height()+seperationPadding;
	m_description2.setTranslation((960-m_description2.width())/2.f, boxOffsetY);

	boxOffsetY -= m_description3.height()+seperationPadding;
	m_description3.setTranslation((960-m_description3.width())/2.f, boxOffsetY);

	auto ytop = boxOffsetY-seperationPadding;
	auto ybottom = (544.f - m_rectangle.height())/2.f+m_nextPageDirection.height()+seperationPadding;
	auto height = ytop - ybottom;
	m_checkbox.setTranslation(boxOffsetX, boxOffsetY-height/2.f+m_checkBoxLabel.height()/2.f);

	// centre within bottom box
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - m_rectangle.height())/2.f);
}

void OfflinePage::update(float dt)
{
	m_checkbox.update(dt);
}

void OfflinePage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_description);
	m_textRenderer.draw(ctx, camera, &m_description2);
	m_textRenderer.draw(ctx, camera, &m_description3);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
	m_checkbox.draw(ctx, camera);
}

void OfflinePage::onEvent(ButtonEvent *event)
{
	if (event->buttons() == SCE_CTRL_CROSS)
	{
		m_checkbox.toggle();
	}
}

bool OfflinePage::installOffline(void) const
{
	return m_checkbox.checked();
}
