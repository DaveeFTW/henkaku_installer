/*
 * installoptionpage.cpp - let the user decide between simple or advanced install
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "installoptionpage.h"

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
	, m_font12("rsc:/fonts/DroidSans.ttf")
	, m_stateMachine(Selection::Simple)
{
	m_font18.setPointSize(18.f);
	m_font16.setPointSize(12.f);
	m_font12.setPointSize(8.f);
	
	m_rectangle.setColour(glm::vec4(0.f, 0.f, 0.f, 0.5f));
	m_selectionBox.setColour(glm::vec4(0.3f, 1.f, 0.2f, 0.2f));

	m_titleText.setText("Select an option:");
	m_titleText.setFont(&m_font18);
	m_titleText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_nextPageDirection.setText("Press right to continue.");
	m_nextPageDirection.setFont(&m_font12);
	m_nextPageDirection.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_simpleInstallationText.setText("Simple Installation");
	m_simpleInstallationText.setFont(&m_font16);
	m_simpleInstallationText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_simpleInstallationDesc.setText("Enables homebrews. Advanced features disabled to protect your device.");
	m_simpleInstallationDesc.setFont(&m_font12);
	m_simpleInstallationDesc.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_customInstallationText.setText("Advanced Installation");
	m_customInstallationText.setFont(&m_font16);
	m_customInstallationText.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_customInstallationDesc.setText("Only for advanced users. Enable potentially dangerous options.");
	m_customInstallationDesc.setFont(&m_font12);
	m_customInstallationDesc.setColour(glm::vec4(0xf7/255.f, 0xb6/255.f, 0x54/255.f, 1.f));

	m_selectionBox.setHeight(m_simpleInstallationText.height()+m_simpleInstallationDesc.height()+(544.f * (5.f/100.f))*3.f);

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

	m_stateMachine.configure(Selection::Simple)
		.on_entry(std::bind(&InstallOptionPage::updateSelectionModel, this))
		.permit(Trigger::Up, Selection::Custom)
		.permit(Trigger::Down, Selection::Custom);

	m_stateMachine.configure(Selection::Custom)
		.on_entry(std::bind(&InstallOptionPage::updateSelectionModel, this))
		.permit(Trigger::Up, Selection::Simple)
		.permit(Trigger::Down, Selection::Simple);

	m_stateMachine.on_transition([this](auto& t)
	{
		LOG(INFO) << "Install option transition from [" << (int)t.source() << "] to [" << (int)t.destination() << "] via trigger [" << (int)t.trigger() << "]";
	});
}

void InstallOptionPage::onModelChanged(glm::mat4 model)
{
	constexpr auto seperationPadding = 544.f * (5.f/100.f);
	auto boxOffsetX = (960.f - m_rectangle.width())/2.f;
	auto boxOffsetY = (544.f + m_rectangle.height())/2.f;

	m_rectangle.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_rectangle.width())/2.f-m_rectangle.radius(), (544-m_rectangle.height())/2.f-m_rectangle.radius(), 0)));

	// pin title centred within box
	boxOffsetY -= m_titleText.height();
	m_titleText.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_titleText.width())/2.f, boxOffsetY, 0)));
	
	boxOffsetY -= m_simpleInstallationText.height() + seperationPadding*2;
	m_simpleInstallationText.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX, boxOffsetY, 0)));
	
	boxOffsetY -= m_simpleInstallationDesc.height() + seperationPadding;
	m_simpleInstallationDesc.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX, boxOffsetY, 0)));
	
	boxOffsetY -= m_customInstallationText.height() + seperationPadding*2;
	m_customInstallationText.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX, boxOffsetY, 0)));

	boxOffsetY -= m_customInstallationDesc.height() + seperationPadding;
	m_customInstallationDesc.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX, boxOffsetY, 0)));

	if (selection() == Selection::Custom)
		m_selectionBox.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX-m_rectangle.radius(), boxOffsetY-seperationPadding, 0)));

	m_nextPageDirection.setModel(model * glm::translate(glm::mat4(1), glm::vec3((960-m_nextPageDirection.width())/2.f, (544.f - m_rectangle.height())/2.f, 0)));
	
	updateSelectionModel();
}

void InstallOptionPage::updateSelectionModel(void)
{
	auto model = modelMatrix();

	constexpr auto seperationPadding = 544.f * (5.f/100.f);
	auto boxOffsetX = (960.f - m_rectangle.width())/2.f;
	auto boxOffsetY = (544.f + m_rectangle.height())/2.f;
	boxOffsetY -= m_titleText.height();
	boxOffsetY -= m_simpleInstallationText.height() + seperationPadding*2;
	boxOffsetY -= m_simpleInstallationDesc.height() + seperationPadding;

	if (selection() == Selection::Custom)
	{
		boxOffsetY -= m_customInstallationText.height() + seperationPadding*2;
		boxOffsetY -= m_customInstallationDesc.height() + seperationPadding;
	}

	m_selectionBox.setModel(model * glm::translate(glm::mat4(1), glm::vec3(boxOffsetX-m_rectangle.radius(), boxOffsetY-seperationPadding, 0)));
}

float g_alphaHah = 0.2f;

void InstallOptionPage::update(float dt)
{
	g_alphaHah = std::fmod(g_alphaHah + dt/10.f, 0.2f);
	m_selectionBox.setColour(glm::vec4(146.f/255.f, 222.f/255.f, 239.f/255.f, g_alphaHah));
	
}

void InstallOptionPage::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_renderer.draw(ctx, camera, &m_rectangle);
	m_renderer.draw(ctx, camera, &m_selectionBox);
	m_textRenderer.draw(ctx, camera, &m_titleText);
	m_textRenderer.draw(ctx, camera, &m_nextPageDirection);
	m_textRenderer.draw(ctx, camera, &m_simpleInstallationText);
	m_textRenderer.draw(ctx, camera, &m_simpleInstallationDesc);
	m_textRenderer.draw(ctx, camera, &m_customInstallationText);
	m_textRenderer.draw(ctx, camera, &m_customInstallationDesc);
}

InstallOptionPage::Selection InstallOptionPage::selection(void) const
{
	return m_stateMachine.state();
}

void InstallOptionPage::onEvent(ButtonEvent *event)
{
	if (event->buttons() == SCE_CTRL_UP)
	{
		m_stateMachine.fire(Trigger::Up);
	}
	else if (event->buttons() == SCE_CTRL_DOWN)
	{
		m_stateMachine.fire(Trigger::Down);
	}
}
