/*
 * installoptionpage.cpp - let the user decide between simple or advanced install
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "installoptionpage.h"
#include "menu.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

InstallOptionPage::InstallOptionPage(GxmShaderPatcher *patcher)
	: m_rectangle((960-200), 400, 20)
	, m_selectionBox(m_rectangle.width()+m_rectangle.radius()*2.f, 200)
	, m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font18("rsc:/fonts/DroidSans.ttf")
	, m_font16("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
	, m_menu(&m_renderer, &m_textRenderer)
{
	m_font18.setPointSize(18.f);
	m_font16.setPointSize(12.f);
	m_font8.setPointSize(8.f);

	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));
	m_selectionBox.setColour(glm::vec4(0.3f, 1.f, 0.2f, 0.2f));

	m_titleText.setText("Select an option:");
	m_titleText.setFont(&m_font18);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press right to continue.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_simpleInstallationText.setText("Simple Installation");
	m_simpleInstallationText.setFont(&m_font16);
	m_simpleInstallationText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_simpleInstallationDesc.setText("Enables homebrews. Advanced features disabled to protect your device.");
	m_simpleInstallationDesc.setFont(&m_font8);
	m_simpleInstallationDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_customInstallationText.setText("Advanced Installation");
	m_customInstallationText.setFont(&m_font16);
	m_customInstallationText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_customInstallationDesc.setText("Only for advanced users. Enable potentially dangerous options.");
	m_customInstallationDesc.setFont(&m_font8);
	m_customInstallationDesc.setColour(glm::vec4(0xf7/255.f, 0xb6/255.f, 0x54/255.f, 1.f));

	auto simpleId = m_menu.add({&m_simpleInstallationText, &m_simpleInstallationDesc});
	auto customId = m_menu.add({&m_customInstallationText, &m_customInstallationDesc});
	m_menu.setSelectionWidth(m_rectangle.width() + m_rectangle.radius()*2.f, -m_rectangle.radius());

	// map these ids to an enum
	m_selectionMap.insert({simpleId, Selection::Simple});
	m_selectionMap.insert({customId, Selection::Custom});

	m_selectionBox.setHeight(m_simpleInstallationText.height()+m_simpleInstallationDesc.height()+(544.f * (5.f/100.f))*3.f);

	// position our components
	positionComponents();
	onModelChanged(modelMatrix());

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

void InstallOptionPage::onModelChanged(glm::mat4 model)
{
	m_rectangle.setWorldMatrix(model);
	m_selectionBox.setWorldMatrix(model);
	m_titleText.setWorldMatrix(model);
	m_nextPageDirection.setWorldMatrix(model);
	m_menu.setWorldMatrix(model);
}

void InstallOptionPage::positionComponents(void)
{
	auto boxOffsetX = (960.f - m_rectangle.width())/2.f;
	auto boxOffsetY = (544.f + m_rectangle.height())/2.f;

	// centre within view
	m_rectangle.setTranslation((960-m_rectangle.width())/2.f-m_rectangle.radius(), (544-m_rectangle.height())/2.f-m_rectangle.radius());

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setTranslation((960-m_titleText.width())/2.f, boxOffsetY);
	m_menu.setTranslation(boxOffsetX, boxOffsetY);
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - m_rectangle.height())/2.f);
}

void InstallOptionPage::update(float dt)
{
	m_menu.update(dt);
}

void InstallOptionPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
	m_menu.draw(ctx, camera);
}

InstallOptionPage::Selection InstallOptionPage::selection(void) const
{
	return m_selectionMap.at(m_menu.selection());
}

void InstallOptionPage::onEvent(Event *event)
{
	if (event->type() == Event::Button)
	{
		ButtonEvent *buttonEvent = reinterpret_cast<ButtonEvent *>(event);

		if (buttonEvent->buttons() == SCE_CTRL_UP)
		{
			m_menu.up();
		}
		else if (buttonEvent->buttons() == SCE_CTRL_DOWN)
		{
			m_menu.down();
		}
	}
}
