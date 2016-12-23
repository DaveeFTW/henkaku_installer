/*
 * installerview.cpp - main UI view
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "installerview.h"
#include "camera.h"
#include "animatedbackground.h"
#include "fpscounter.h"
#include "welcomepage.h"

#include <framework/task.h>
#include <framework/buttonevent.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <psp2/ctrl.h>

#include <easyloggingpp/easylogging++.h>

namespace
{
	template <typename T, typename B>
	T buttonToTrigger(B buttons)
	{
		switch (buttons)
		{
		case SCE_CTRL_LEFT:
			return T::Left;
		case SCE_CTRL_RIGHT:
			return T::Right;
		case SCE_CTRL_UP:
			return T::Up;
		case SCE_CTRL_DOWN:
			return T::Down;
		case SCE_CTRL_CROSS:
			return T::Cross;
		default:
			return T::None;
		}
	}
} // anonymous namespace

InstallerView::InstallerView(void)
	: m_animatedBackground(new AnimatedBackground(&m_patcher))
	, m_fpsCounter(new FpsCounter(&m_patcher))
	, m_camera(new Camera)
	, m_stateMachine(State::Init)
{
	// align z = 0 to screen coordiantes
	auto fov = 45.f;
	m_camera->setPerspectiveProjection(glm::radians(fov), 960.f/544.f, 0.1f, 10000.0f);
	float angle1=fov/2.0;
	float angle2=180 - (90 + angle1);
	float Z = 0.5 * 544.f * std::sin(glm::radians(angle2))/std::sin(glm::radians(angle1));
	m_camera->setPosition(glm::vec3(960.f/2, 544.f/2, Z));
	m_camera->setViewCenter(glm::vec3(960.f/2, 544.f/2, 0));
	m_camera->setUpVector(glm::vec3(0, 1, 0));

	m_fpsCounter->setModel(glm::scale(glm::mat4(1), glm::vec3(0.7, 0.7, 0)));

	m_simulationTasks = std::make_shared<Task>();
	m_simulationTasks->set(std::bind(&InstallerView::update, this));

	auto animatedBackgroundTask = std::make_shared<Task>();
	animatedBackgroundTask->set(std::bind(&AnimatedBackground::update, m_animatedBackground, std::cref(m_dt)));

	auto fpsCounterTask = std::make_shared<Task>();
	fpsCounterTask->set(std::bind(&FpsCounter::update, m_fpsCounter, std::cref(m_dt)));

	// set pages
	auto welcomePage = new WelcomePage(&m_patcher);
	auto welcomePageTask = std::make_shared<Task>();
	welcomePageTask->set(std::bind(&WelcomePage::update, welcomePage, std::cref(m_dt)));

	m_pages.insert({ State::Welcome, welcomePage });

	// setup state machine
	m_stateMachine.configure(State::Init)
		.permit(Trigger::Start, State::Welcome);

	// add update tasks
	m_simulationTasks->insertDependant(animatedBackgroundTask);
	m_simulationTasks->insertDependant(fpsCounterTask);
	m_simulationTasks->insertDependant(welcomePageTask);

	// goto next
	m_stateMachine.fire(Trigger::Start);
}

InstallerView::~InstallerView(void)
{
	for (auto& page : m_pages)
	{
		delete page.second;
	}

	m_pages.clear();

	delete m_camera;
	delete m_animatedBackground;
}

void InstallerView::onEvent(Event *event)
{
	if (event->type() == Event::Button)
	{
		ButtonEvent *button = reinterpret_cast<ButtonEvent *>(event);
		
		auto trigger = buttonToTrigger<Trigger>(button->buttons());

		// check if this input will trigger a state change
		if (m_stateMachine.can_fire(trigger))
		{
			m_stateMachine.fire(trigger);
		}
	}
}

TaskPtr InstallerView::simulationTask(double dt)
{
	m_dt = dt;
	return m_simulationTasks;
}

void InstallerView::update(void)
{
}

void InstallerView::render(SceGxmContext *ctx)
{
	m_animatedBackground->draw(ctx, m_camera);
	m_fpsCounter->draw(ctx, m_camera);

	for (auto& page : m_pages)
	{
		page.second->draw(ctx, m_camera);
	}
}

bool InstallerView::isTransitioning(void) const
{
	return m_isTransitioning;
}
