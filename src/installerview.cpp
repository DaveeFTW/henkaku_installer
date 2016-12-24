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
#include "installoptionpage.h"

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
	setupCamera();

	m_fpsCounter->setModel(glm::scale(glm::mat4(1), glm::vec3(0.7, 0.7, 0)));

	m_simulationTasks = std::make_shared<Task>();
	m_simulationTasks->set(std::bind(&InstallerView::update, this));

	auto animatedBackgroundTask = std::make_shared<Task>();
	animatedBackgroundTask->set(std::bind(&AnimatedBackground::update, m_animatedBackground, std::cref(m_dt)));

	auto fpsCounterTask = std::make_shared<Task>();
	fpsCounterTask->set(std::bind(&FpsCounter::update, m_fpsCounter, std::cref(m_dt)));

	// add update tasks
	m_simulationTasks->insertDependant(animatedBackgroundTask);
	m_simulationTasks->insertDependant(fpsCounterTask);

	// create state transition guard
	m_transitionGuard = [this](void)
	{
		return !this->isTransitioning();
	};

	// setup pages and states
	setupWelcomePage();
	setupInstallOptionPage();

	//auto welcomePage2 = new WelcomePage(&m_patcher);

	//m_pages.insert({ State::SimpleInstall, welcomePage2 });

	// setup state machine
	m_stateMachine.configure(State::Init)
		.permit(Trigger::Start, State::Welcome);

	//m_stateMachine.configure(State::SimpleInstall)
	//	.permit_if(Trigger::Left, State::Welcome, m_transitionGuard);

	setupTransitionPan();

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
		ButtonEvent button = m_buttonFilter.filter(reinterpret_cast<ButtonEvent *>(event));
		
		auto trigger = buttonToTrigger<Trigger>(button.buttons());

		// check if this input will trigger a state change
		if (m_stateMachine.can_fire(trigger))
		{
			m_stateMachine.fire(trigger);
		}
		else if (m_pages.count(m_stateMachine.state()))
		{
			m_pages.at(m_stateMachine.state())->onEvent(&button);
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
	m_cameraPanX.update(m_dt);
	m_cameraPanY.update(m_dt);
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

void InstallerView::setupCamera(void)
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
}

void InstallerView::setupTransitionPan(void)
{
	m_cameraPanX.setDuration(300);
	m_cameraPanY.setDuration(300);

	// set step handlers
	m_cameraPanX.setStepHandler([this](auto step)
	{
		// update camera position
		auto pos = this->m_camera->position();
		this->m_camera->setPosition(glm::vec3(step, pos.y, pos.z));

		// update camera view
		auto view = this->m_camera->viewCenter();
		this->m_camera->setViewCenter(glm::vec3(step, view.y, view.z));
	});

	m_cameraPanY.setStepHandler([this](auto step)
	{
		// update camera position
		auto pos = this->m_camera->position();
		this->m_camera->setPosition(glm::vec3(pos.x, step, pos.z));

		// update camera view
		auto view = this->m_camera->viewCenter();
		this->m_camera->setViewCenter(glm::vec3(view.x, step, view.z));
	});

	// use quadratic in
	m_cameraPanX.setEasing([](float t, float b, float c, float d)
	{
		t /= d;
		return -c * t*(t-2) + b;
	});

	m_cameraPanY.setEasing([](float t, float b, float c, float d)
	{
		t /= d;
		return -c * t*(t-2) + b;
	});

	// we only need one of these
	m_cameraPanX.setCompletionHandler([this](void)
	{
		m_isTransitioning = false;
	});

	m_stateMachine.on_transition([this](auto& transition)
	{
		this->performPageTransition(transition);
	});
}

void InstallerView::performPageTransition(const StateTransition& t)
{
	LOG(INFO) << "transition from [" << (int)t.source() << "] to [" << (int)t.destination() << "] via trigger [" << (int)t.trigger() << "]";

	auto source = t.source();
	auto dest = t.destination();

	// if source is not a page we do nothing
	// TODO: if dest is not a page we should error or something
	if (!this->m_pages.count(source))
	{
		return;
	}

	auto position = this->m_camera->position();
	auto destPosition = this->m_pages.at(dest)->modelMatrix() * glm::vec4(1.f);
	auto sourcePosition = this->m_pages.at(source)->modelMatrix() * glm::vec4(1.f);
	auto distance = destPosition - sourcePosition;

	// setup x axis pan
	this->m_cameraPanX.setStart(position.x);
	this->m_cameraPanX.setEnd(distance.x);

	// setup y axis pan
	this->m_cameraPanY.setStart(position.y);
	this->m_cameraPanY.setEnd(distance.y);

	m_isTransitioning = true;
	this->m_cameraPanX.start();
	this->m_cameraPanY.start();
}

void InstallerView::setupWelcomePage(void)
{
	auto page = new WelcomePage(&m_patcher);
	
	// add task as dependant on this view
	auto task = std::make_shared<Task>();
	task->set(std::bind(&WelcomePage::update, page, std::cref(m_dt)));
	m_simulationTasks->insertDependant(task);

	// add page to map
	m_pages.insert({ State::Welcome, page });

	// setup our state transitions
	m_stateMachine.configure(State::Welcome)
		.permit_if(Trigger::Right, State::SelectInstallOption, m_transitionGuard);
}

void InstallerView::setupInstallOptionPage(void)
{
	auto page = new InstallOptionPage(&m_patcher);
	page->setModel(glm::translate(glm::mat4(1), glm::vec3(960.f*1.5f, 0, 0)));
	
	// add task as dependant on this view
	auto task = std::make_shared<Task>();
	task->set(std::bind(&InstallOptionPage::update, page, std::cref(m_dt)));
	m_simulationTasks->insertDependant(task);

	// add page to map
	m_pages.insert({ State::SelectInstallOption, page });

	// setup our state transitions
	m_stateMachine.configure(State::SelectInstallOption)
		.permit_if(Trigger::Left, State::Welcome, m_transitionGuard)
		.permit_dynamic_if(Trigger::Cross, m_transitionGuard, [this, page](auto& source)
		{
			if (page->selection() == InstallOptionPage::Selection::Simple)
			{
				// set default options
				this->m_henkakuOptions.unsafeHomebrew = true;
				this->m_henkakuOptions.versionSpoofing = true;
				this->m_henkakuOptions.offlineInstaller = false;
				this->m_henkakuOptions.resetAll = false;
				return State::Install;
			}

			// TODO: check if henkaku config exists
			auto configExists = true;
			return configExists ? State::Reset : State::Config;
		});
}

void InstallerView::setupResetPage(void)
{

}
