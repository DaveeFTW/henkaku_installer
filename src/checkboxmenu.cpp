/*
 * checkboxmenu.cpp - simple list menu with checkbox elements. ideally we would make menu generic instead
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */


#include "checkboxmenu.h"
#include "checkbox.h"
#include "geometryrenderer.h"
#include "text.h"
#include "easingcurves.h"

CheckBoxMenu::CheckBoxMenu(GeometryRenderer *geometryRenderer, GeometryRenderer *textRenderer)
	: m_geometryRenderer(geometryRenderer)
	, m_textRenderer(textRenderer)
	, m_stateMachine(-1)
	, m_selectionBoxOffset(0)
	, m_usingSelectedWidth(false)
{
	onModelChanged(modelMatrix());

	m_selectionGlow.setStart(0.3f);
	m_selectionGlow.setEnd(-0.3f);
	m_selectionGlow.setDuration(1000);

	m_selectionGlow.setEasing([this](auto t, auto b, auto c, auto d)
	{
		using namespace easing::quad;
		return this->m_rising ? (out(t, b, c, d)) : (in(t, b, c, d));
	});

	m_selectionGlow.setStepHandler([this](auto step)
	{
		this->m_selectionBox.setColour(glm::vec4(146.f/255.f, 222.f/255.f, 239.f/255.f, step));
	});

	m_selectionGlow.setCompletionHandler([this](void)
	{
		this->m_rising = !this->m_rising;
		this->m_selectionGlow.setStart((m_rising) ? (0.0f) : (0.3f));
		this->m_selectionGlow.setEnd((m_rising) ? (0.3f) : (-0.3f));
		this->m_selectionGlow.start();
	});

	m_rising = false;
	m_selectionGlow.start();

	m_selectionY.setEasing([this](auto t, auto b, auto c, auto d)
	{
		using namespace easing::quad;
		return inOut(t, b, c, d);
	});

	m_selectionY.setStepHandler([this](auto step)
	{
		this->positionSelectionBox(step);
	});

	m_selectionY.setDuration(200);

	m_stateMachine.on_transition([this](auto& t)
	{
		this->m_selectionY.setStart(t.source());
		this->m_selectionY.setEnd(t.destination()-t.source());
		this->m_selectionY.start();
	});
}

int CheckBoxMenu::add(Item item)
{
	m_items.push_back(item);
	positionComponents();

	// allow down presses to move to the new element
	m_stateMachine.configure(static_cast<int>(m_items.size())-2)
		.permit(Trigger::Down, m_items.size()-1);

	// check if there is a valid element before to create an up action
	if (m_items.size() > 1)
	{
		m_stateMachine.configure(m_items.size()-1)
			.permit(Trigger::Up, m_items.size()-2);
	}
	else
	{
		// this is the first item added, we just fire to move the selection box
		m_stateMachine.fire(Trigger::Down);
	}

	return m_items.size()-1;
}

int CheckBoxMenu::size(void) const
{
	return m_items.size();
}

float CheckBoxMenu::width(void) const
{
	return m_width;
}

float CheckBoxMenu::height(void) const
{
	return m_height;
}

void CheckBoxMenu::up(void)
{
	if (m_stateMachine.can_fire(Trigger::Up))
		m_stateMachine.fire(Trigger::Up);
}

void CheckBoxMenu::down(void)
{
	if (m_stateMachine.can_fire(Trigger::Down))
		m_stateMachine.fire(Trigger::Down);
}

void CheckBoxMenu::toggle(void)
{
	m_items.at(selection()).title->toggle();
}

int CheckBoxMenu::selection(void) const
{
	return m_stateMachine.state();
}

void CheckBoxMenu::setSelectionWidth(float width, float xoffset)
{
	m_usingSelectedWidth = true;
	m_selectionBoxOffset = xoffset;
	m_selectionBox.setWidth(width);
	positionSelectionBox(m_stateMachine.state());
}

void CheckBoxMenu::onModelChanged(glm::mat4 model)
{
	m_selectionBox.setWorldMatrix(model);

	for (auto& item : m_items)
	{
		item.title->setWorldMatrix(model);
		item.desc->setWorldMatrix(model);
	}
}

void CheckBoxMenu::positionComponents(void)
{
	constexpr auto seperationPadding = 544.f * (5.f/100.f);
	auto yoffset = 0.f;
	auto width = 0.f;
	auto componentHeight = 0.f;

	// first calculate width/height
	for (auto& item : m_items)
	{
		auto titleHeight = item.title->height()+seperationPadding*2;
		auto descHeight = item.desc->height()+seperationPadding;
		width = std::max(width, item.title->width());
		width = std::max(width, item.desc->width());
		componentHeight = std::max(componentHeight, titleHeight+descHeight);
	}

	if (!m_usingSelectedWidth)
		m_selectionBox.setWidth(m_width);

	m_selectionBox.setHeight(componentHeight);

	m_width = width;
	m_height = m_items.size()*componentHeight;
	m_componentHeight = componentHeight;

	// now translation
	for (auto& item : m_items)
	{
		auto titleHeight = item.title->height()+seperationPadding*2;
		auto descHeight = item.desc->height()+seperationPadding;

		yoffset -= titleHeight;
		item.title->setTranslation(0, yoffset+item.title->height());

		yoffset -= descHeight;
		item.desc->setTranslation(0, yoffset);

		yoffset -= componentHeight - (titleHeight + descHeight);
	}
}

void CheckBoxMenu::positionSelectionBox(float y)
{
	constexpr auto seperationPadding = 544.f * (5.f/100.f);
	m_selectionBox.setTranslation(m_selectionBoxOffset, -(y+1)*m_componentHeight - seperationPadding);
}

void CheckBoxMenu::update(float dt)
{
	m_selectionGlow.update(dt);
	m_selectionY.update(dt);
	
	for (auto& item : m_items)
	{
		item.title->update(dt);
	}
}

void CheckBoxMenu::draw(SceGxmContext *ctx, const Camera *camera) const
{
	if (m_items.size())
		m_geometryRenderer->draw(ctx, camera, &m_selectionBox);

	for (auto& item : m_items)
	{
		item.title->draw(ctx, camera);
		m_textRenderer->draw(ctx, camera, item.desc);
	}
}
