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
#include "staticbackground.h"
#include "fpscounter.h"
#include "intropage.h"
#include "welcomepage.h"
#include "warningpage.h"
#include "installoptionpage.h"
#include "resetpage.h"
#include "configpage.h"
#include "offlinepage.h"
#include "confirmpage.h"
#include "installpage.h"
#include "successpage.h"
#include "failurepage.h"
#include "focusinevent.h"
#include "easingcurves.h"

#include <framework/task.h>
#include <framework/buttonevent.h>
#include <framework/guiapplication.h>

#include <sys/stat.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <easyloggingpp/easylogging++.h>

namespace
{
	template <typename T, typename B>
	T buttonToTrigger(B buttons)
	{
		switch (buttons)
		{
		case SCE_CTRL_SELECT:
			return T::Select;
		case SCE_CTRL_L3:
			return T::L3;
		case SCE_CTRL_R3:
			return T::R3;
		case SCE_CTRL_START:
			return T::Start;
		case SCE_CTRL_UP:
			return T::Up;
		case SCE_CTRL_RIGHT:
			return T::Right;
		case SCE_CTRL_DOWN:
			return T::Down;
		case SCE_CTRL_LEFT:
			return T::Left;
		case SCE_CTRL_LTRIGGER:
			return T::LTrigger;
		case SCE_CTRL_RTRIGGER:
			return T::RTrigger;
		case SCE_CTRL_L1:
			return T::L1;
		case SCE_CTRL_R1:
			return T::R1;
		case SCE_CTRL_TRIANGLE:
			return T::Triangle;
		case SCE_CTRL_CIRCLE:
			return T::Circle;
		case SCE_CTRL_CROSS:
			return T::Cross;
		case SCE_CTRL_SQUARE:
			return T::Square;
		default:
			return T::None;
		}
	}
} // anonymous namespace

InstallerView::InstallerView(void)
	: m_animatedBackground(new AnimatedBackground(&m_patcher))
	, m_staticBackground(new StaticBackground(&m_patcher))
	, m_fpsCounter(new FpsCounter(&m_patcher))
	, m_camera(new Camera)
	, m_stateMachine(State::Init)
{
	setupCamera();

	//m_fpsCounter->setModel(glm::scale(glm::mat4(1), glm::vec3(0.7, 0.7, 0)));

	m_simulationTasks = std::make_shared<Task>();

	auto animatedBackgroundTask = std::make_shared<Task>();
	animatedBackgroundTask->set(std::bind(&AnimatedBackground::update, m_animatedBackground, std::cref(m_dt)));

	auto fpsCounterTask = std::make_shared<Task>();
	fpsCounterTask->set(std::bind(&FpsCounter::update, m_fpsCounter, std::cref(m_dt)));

	auto localTask = std::make_shared<Task>();
	localTask->set(std::bind(&InstallerView::update, this, std::cref(m_dt)));

	// add update tasks
	m_simulationTasks->insertDependant(animatedBackgroundTask);
	m_simulationTasks->insertDependant(fpsCounterTask);
	m_simulationTasks->insertDependant(localTask);

	// create state transition guard
	m_transitionGuard = [this](void)
	{
		return !this->isTransitioning();
	};

	// setup pages and states
	setupIntroPage(0, 0);
	setupWelcomePage(-1, 0);
	setupWarningPage(-1, 0);
	setupInstallOptionPage(0, 0);
	setupResetPage(-1, 0);
	setupConfigPage(0, 0);
	setupOfflinePage(1, 0);
	setupConfirmPage(2, 0);
	setupInstallPage(3, 0);
	setupSuccessPage(4, 0);
	setupFailurePage(1, 0);

	// setup state machine
	m_stateMachine.configure(State::Init)
		.permit(Trigger::Start, State::Intro);

	// quickly exit by using sceKernelExitProcess
	// ideally we would GuiApplication::exit, but the time it takes to deallocate
	// just isnt worth it
	m_stateMachine.configure(State::Exit)
		.on_entry(std::bind(&sceKernelExitProcess, 0));

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
	delete m_staticBackground;
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
		else if (trigger != Trigger::None && m_stateMachine.can_fire(Trigger::OtherButtonPress))
		{
			m_stateMachine.fire(Trigger::OtherButtonPress);
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

void InstallerView::update(float dt)
{
	m_cameraPanX.update(dt);
	m_cameraPanY.update(dt);

	for (auto& page : m_pages)
	{
		page.second->update(dt);
	}
}

void InstallerView::render(SceGxmContext *ctx)
{
	m_staticBackground->draw(ctx, m_camera);
	m_fpsCounter->draw(ctx, m_camera);

	for (auto& page : m_renderQueue)
	{
		page->draw(ctx, m_camera);
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
	m_camera->setPosition(glm::vec3(960.f/2 - 1, 544.f/2, Z));
	m_camera->setViewCenter(glm::vec3(960.f/2 - 1, 544.f/2, 0));
	m_camera->setUpVector(glm::vec3(0, 1, 0));
}

void InstallerView::setupTransitionPan(void)
{
	m_cameraPanX.setDuration(1000);
	m_cameraPanY.setDuration(1000);

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

	// use quadratic out
	m_cameraPanX.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	m_cameraPanY.setEasing([](float t, float b, float c, float d)
	{
		return easing::quad::out(t, b, c, d);
	});

	// we only need one of these
	m_cameraPanX.setCompletionHandler([this](void)
	{
		this->m_renderQueue.pop_front();
		m_isTransitioning = false;
		FocusInEvent focusIn;
		this->m_pages.at(this->m_stateMachine.state())->onEvent(&focusIn);
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

	if (!this->m_pages.count(source) || !this->m_pages.count(dest))
	{
		// if source is not a page we just move the camera directly to the dest
		if (this->m_pages.count(dest))
		{
			this->m_renderQueue.push_back(this->m_pages.at(dest));

			auto destPosition = this->m_pages.at(dest)->modelMatrix() * glm::vec4(1.f);

			// update camera position
			auto pos = this->m_camera->position() + glm::vec3(destPosition.x, destPosition.y, 0.f);
			this->m_camera->setPosition(pos);

			// update camera view
			auto view = this->m_camera->viewCenter() + glm::vec3(destPosition.x, destPosition.y, 0.f);
			this->m_camera->setViewCenter(view);

			// tell the page that it has focus
			FocusInEvent focusIn;
			this->m_pages.at(dest)->onEvent(&focusIn);
		}

		return;
	}

	// add page to render queue
	this->m_renderQueue.push_back(this->m_pages.at(dest));

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

bool InstallerView::resetAvailable(void) const
{
	// check if henkaku config exists
	struct stat buffer;
	auto henkakuConfigExists = stat("ux0:temp/app_work/MLCL00001/rec/config.bin", &buffer) == 0;
	auto taihenConfigExists = stat("ux0:tai/config.txt", &buffer) == 0;
	return henkakuConfigExists || taihenConfigExists;
}

void InstallerView::setupIntroPage(int x, int y)
{
	auto page = new IntroPage(&m_patcher);
	page->setTranslation((960-960/2)*x, (960-960/2)*y);

	// add page to map
	m_pages.insert({ State::Intro, page });

	// setup our state transitions
	m_stateMachine.configure(State::Intro)
		.permit_if(Trigger::TaskComplete, State::SelectInstallOption, m_transitionGuard);

	page->setExitTrigger([this]()
	{
		this->m_stateMachine.fire(Trigger::TaskComplete);
	});
}

void InstallerView::setupWelcomePage(int x, int y)
{
	auto page = new WelcomePage(&m_patcher);
	page->setTranslation((960-960/2)*x, (960-960/2)*y);

	// add page to map
	m_pages.insert({ State::Welcome, page });

	// setup our state transitions
	m_stateMachine.configure(State::Welcome)
		.permit_if(Trigger::TaskComplete, State::WarningMessage, m_transitionGuard);

	page->setExitTrigger([this]()
	{
		this->m_stateMachine.fire(Trigger::TaskComplete);
	});
}

void InstallerView::setupWarningPage(int x, int y)
{
	auto page = new WarningPage(&m_patcher);
	page->setTranslation((960-960/2)*x, (960-960/2)*y);

	// add page to map
	m_pages.insert({ State::WarningMessage, page });

	auto warningPageTransitionGuard = [this, page](void)
	{
		return this->m_transitionGuard() && page->transitionGuard();
	};

	// setup our state transitions
	m_stateMachine.configure(State::WarningMessage)
		.permit_if(Trigger::Circle, State::SelectInstallOption, warningPageTransitionGuard)
		.permit_if(Trigger::OtherButtonPress, State::Exit, warningPageTransitionGuard);
}

void InstallerView::setupInstallOptionPage(int x, int y)
{
	auto page = new InstallOptionPage(&m_patcher);
	page->setTranslation((960-960/2)*x, (960-960/2)*y);

	// add page to map
	m_pages.insert({ State::SelectInstallOption, page });

	// setup our state transitions
	m_stateMachine.configure(State::SelectInstallOption)
		.permit_if(Trigger::Left, State::Welcome, m_transitionGuard)
		.permit_dynamic_if(Trigger::Right, m_transitionGuard, [this, page](auto& source)
		{
			//if (page->selection() == InstallOptionPage::Selection::Simple)
			{
				// set default options
				this->m_henkakuOptions.unsafeHomebrew = true;
				this->m_henkakuOptions.versionSpoofing = true;
				this->m_henkakuOptions.offlineInstaller = false;
				this->m_henkakuOptions.resetAll = false;
				return State::Install;
			}

			// present reset page if config is available
			return this->resetAvailable() ? State::Reset : State::Config;
		});
}

void InstallerView::setupResetPage(int x, int y)
{
	auto page = new ResetPage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Reset, page });

	// setup our state transitions
	m_stateMachine.configure(State::Reset)
		.on_exit([this, page](auto s)
		{
			// nice naming consistency nerd
			this->m_henkakuOptions.resetAll = page->reset();
		})
		.permit_if(Trigger::Left, State::SelectInstallOption, m_transitionGuard)
		.permit_if(Trigger::Right, State::Config, m_transitionGuard);
}

void InstallerView::setupConfigPage(int x, int y)
{
	auto page = new ConfigPage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Config, page });

	// setup our state transitions
	m_stateMachine.configure(State::Config)
		.on_exit([this, page](auto s)
		{
			// nice naming consistency nerd
			this->m_henkakuOptions.unsafeHomebrew = page->unsafeHomebrew() == ConfigPage::UnsafeHomebrew::Enabled;
			this->m_henkakuOptions.versionSpoofing = page->versionSpoofing() == ConfigPage::VersionSpoofing::Enabled;
		})
		.permit_dynamic_if(Trigger::Left, m_transitionGuard, [this](auto& source)
		{
			// present reset page if config is available
			return this->resetAvailable() ? State::Reset : State::SelectInstallOption;
		})
		.permit_if(Trigger::Right, State::Offline, m_transitionGuard);
}

void InstallerView::setupOfflinePage(int x, int y)
{
	auto page = new OfflinePage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Offline, page });

	// setup our state transitions
	m_stateMachine.configure(State::Offline)
		.on_exit([this, page](auto s)
		{
			// nice naming consistency nerd
			this->m_henkakuOptions.offlineInstaller = page->installOffline();
		})
		.permit_if(Trigger::Left, State::Config, m_transitionGuard)
		.permit_if(Trigger::Right, State::Confirm, m_transitionGuard);
}

void InstallerView::setupConfirmPage(int x, int y)
{
	auto page = new ConfirmPage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Confirm, page });

	// setup our state transitions
	m_stateMachine.configure(State::Confirm)
		.on_entry([this, page](auto s)
		{
			page->setConfigurationOptions(this->m_henkakuOptions);
		})
		.permit_if(Trigger::Left, State::Offline, m_transitionGuard)
		.permit_if(Trigger::Cross, State::Install, m_transitionGuard);
}

void InstallerView::setupInstallPage(int x, int y)
{
	auto page = new InstallPage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Install, page });

	// setup our state transitions
	m_stateMachine.configure(State::Install)
		.permit_if(Trigger::Cross, State::Success, m_transitionGuard);
}

void InstallerView::setupSuccessPage(int x, int y)
{
	auto page = new SuccessPage(&m_patcher);
	page->setTranslation(960.f*1.5f*x, 544.f*1.5f*y);

	// add page to map
	m_pages.insert({ State::Success, page });

	// setup our state transitions
	m_stateMachine.configure(State::Success)
		.permit_if(Trigger::Cross, State::Exit, m_transitionGuard);
}

void InstallerView::setupFailurePage(int x, int y)
{
	auto page = new FailurePage(&m_patcher);
	page->setTranslation((960-960/2)*x, (960-960/2)*y);

	// add page to map
	m_pages.insert({ State::Failure, page });
	m_pages.insert({ State::DisableSafeModeError, page });
	m_pages.insert({ State::UpdateHenkakuError, page });
	m_pages.insert({ State::UnknownDataError, page });
	m_pages.insert({ State::InstallationError, page });
	m_pages.insert({ State::UninstallationError, page });

	// setup our state transitions
	m_stateMachine.configure(State::Failure)
		.permit_if(Trigger::OtherButtonPress, State::Exit, m_transitionGuard);

	m_stateMachine.configure(State::DisableSafeModeError)
		.sub_state_of(State::Failure)
		.on_entry([this, page](auto s)
		{
			page->setMessage(
			{
				"Please disable HENkaku Safe Mode from Settings",
				"before running this installer."
			});
		});

	m_stateMachine.configure(State::UpdateHenkakuError)
		.sub_state_of(State::Failure)
		.on_entry([this, page](auto s)
		{
			page->setMessage(
			{
				"Your HENkaku version is too old! Please install",
				"R10 from https://henkaku.xyz/go (not the offline",
				"installer!)"
			});
		});

	m_stateMachine.configure(State::UnknownDataError)
		.sub_state_of(State::Failure)
		.on_entry([this, page](auto s)
		{
			page->setMessage(
			{
				"Unknown data was found and Ensō cannot be safely installed.",
				"A dump was created at %s."
			});
		});

	m_stateMachine.configure(State::InstallationError)
		.sub_state_of(State::Failure)
		.on_entry([this, page](auto s)
		{
			page->setMessage(
			{
				"An error has occured (err code), the log has been",
				"written to %s"
			});
		});

	m_stateMachine.configure(State::UninstallationError)
		.sub_state_of(State::Failure)
		.on_entry([this, page](auto s)
		{
			page->setMessage(
			{
				"An error has occured (err code), the log has been",
				"written to %s"
			});
		});
}
