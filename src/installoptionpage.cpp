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
#include "easingcurves.h"

#include <framework/buttonevent.h>
#include <psp2/ctrl.h>
#include <easyloggingpp/easylogging++.h>

InstallOptionPage::InstallOptionPage(GxmShaderPatcher *patcher, bool ensoInstalled)
	: m_renderer(patcher)
	, m_textRenderer(patcher)
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_font8("rsc:/fonts/DroidSans.ttf")
	, m_menu(&m_renderer, &m_textRenderer)
{
	m_font12.setPointSize(12.f);
	m_font8.setPointSize(8.f);

	m_nextPageDirection.setText("Press CROSS to select.");
	m_nextPageDirection.setFont(&m_font8);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_installText.setText("Install");
	m_installText.setFont(&m_font12);
	m_installText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_installDesc.setText("Install Ensō to this device.");
	m_installDesc.setFont(&m_font8);
	m_installDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_reactivateText.setText("Reactivate");
	m_reactivateText.setFont(&m_font12);
	m_reactivateText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_reactivateDesc.setText("Reactivate the current Ensō installation on this device.");
	m_reactivateDesc.setFont(&m_font8);
	m_reactivateDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_uninstallText.setText("Uninstall");
	m_uninstallText.setFont(&m_font12);
	m_uninstallText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_uninstallDesc.setText("Completely remove this Ensō installation.");
	m_uninstallDesc.setFont(&m_font8);
	m_uninstallDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_exitText.setText("Exit");
	m_exitText.setFont(&m_font12);
	m_exitText.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	m_exitDesc.setText("Exit the installer without doing anything.");
	m_exitDesc.setFont(&m_font8);
	m_exitDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 0.f));

	if (ensoInstalled)
	{
		auto reactivateId = m_menu.add({&m_reactivateText, &m_reactivateDesc});
		auto uninstallId = m_menu.add({&m_uninstallText, &m_uninstallDesc});

		// map these ids to an enum
		m_selectionMap.insert({reactivateId, Selection::Reactivate});
		m_selectionMap.insert({uninstallId, Selection::Uninstall});
	}
	else
	{
		auto installId = m_menu.add({&m_installText, &m_installDesc});

		// map these ids to an enum
		m_selectionMap.insert({installId, Selection::Install});
	}


	auto exitId = m_menu.add({&m_exitText, &m_exitDesc});

	// map these ids to an enum
	m_selectionMap.insert({exitId, Selection::Exit});

	m_menu.setSelectionWidth((960-200) + 20*2.f, -20);
	m_menu.disableSelection();

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

	// animation
	// fade in direction
	m_fadeInMenu.setStart(0.f);
	m_fadeInMenu.setEnd(1.0f);
	m_fadeInMenu.setDuration(750);
	m_fadeInMenu.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeInMenu.setStepHandler([this](float step)
	{
		this->m_installText.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_installDesc.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_reactivateText.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_reactivateDesc.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_uninstallText.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_uninstallDesc.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_exitText.setColour(glm::vec4(1.f, 1.f, 1.f, step));
		this->m_exitDesc.setColour(glm::vec4(1.f, 1.f, 1.f, step));
	});

	m_fadeInMenu.setCompletionHandler([this]()
	{
		m_fadeInDirection.start();
	});

	// fade in direction
	m_fadeInDirection.setStart(0.f);
	m_fadeInDirection.setEnd(1.0f);
	m_fadeInDirection.setDuration(750);
	m_fadeInDirection.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::in(t, b, c, d);
	});

	m_fadeInDirection.setStepHandler([this](float step)
	{
		this->m_nextPageDirection.setColour(glm::vec4(1.0f, 1.f, 1.f, step));
	});

	m_fadeInDirection.setCompletionHandler([this]()
	{
		m_menu.enableSelection();
	});
}

void InstallOptionPage::onModelChanged(glm::mat4 model)
{
	m_nextPageDirection.setWorldMatrix(model);
	m_menu.setWorldMatrix(model);
}

void InstallOptionPage::positionComponents(void)
{
	constexpr auto seperationPadding = 544.f * (2.f/100.f);
	auto boxOffsetX = (960.f - (960-200))/2.f;
	auto boxOffsetY = (544.f - 400.f)/2.f;

	//auto menuHeightOffset = m_menu.height()/2.f;

	auto directionTop = boxOffsetY+m_nextPageDirection.height()+3*seperationPadding;
	auto offset = 0.f;

	if (m_menu.height() < 544.f - directionTop)
	{
		offset = ((544.f - directionTop) - m_menu.height())/2.f;
	}

	m_menu.setTranslation(boxOffsetX, directionTop+offset+m_menu.height());
	m_nextPageDirection.setTranslation((960-m_nextPageDirection.width())/2.f, (544.f - 400.f)/2.f);
}

void InstallOptionPage::update(float dt)
{
	if (!m_fadeInMenu.complete())
		m_fadeInMenu.update(dt);

	if (!m_fadeInDirection.complete())
		m_fadeInDirection.update(dt);

	m_menu.update(dt);
}

void InstallOptionPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
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

	if (event->type() == Event::FocusIn)
	{
		m_menu.disableSelection();
		m_fadeInMenu.start();
	}
}
