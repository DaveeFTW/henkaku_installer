/*
 * installerview.h - main UI view
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "numberanimation.h"
#include "buttoneventfilter.h"

#include <framework/view.h>
#include <framework/gxmshaderpatcher.h>

#include <stateless++/state_machine.hpp>

#include <memory>
#include <unordered_map>

class AnimatedBackground;
class Camera;
class FpsCounter;
class Page;

class InstallerView : public View
{
public:
	InstallerView(void);
	~InstallerView(void);

	TaskPtr simulationTask(double dt) override;
	void render(SceGxmContext *ctx) override;

protected:
	void onEvent(Event *event) override;

private:
	enum class State
	{
		Init,
		Welcome,
		SelectInstallOption,
		SimpleInstall,
		CustomInstall,
		Reset,
		Config,
		Offline,
		Confirm,
		Install,
		Success,
		Failure,
		Exit
	};

	enum class Trigger
	{
		Start,
		Left,
		Right,
		Up,
		Down,
		Cross,
		None
	};

	using StateMachine = stateless::state_machine<State, Trigger>;
	using TransitionGuard = StateMachine::TStateConfiguration::TGuard;
	using StateTransition = StateMachine::TTransition;

	struct HenkakuOptions
	{
		bool unsafeHomebrew;
		bool versionSpoofing;
		bool offlineInstaller;
		bool resetAll;
	};

private:
	void update(void);
	bool isTransitioning(void) const;
	void performPageTransition(const StateTransition& transition);
	void setupCamera(void);
	void setupTransitionPan(void);
	void setupWelcomePage(void);
	void setupInstallOptionPage(void);
	void setupResetPage(void);
	void setupConfigPage(void);
	void setupOfflinePage(void);
	void setupConfirmPage(void);
	void setupInstallPage(void);
	void setupSuccessPage(void);

private:
	TaskPtr m_simulationTasks;
	GxmShaderPatcher m_patcher;
	AnimatedBackground *m_animatedBackground;
	FpsCounter *m_fpsCounter;
	Camera *m_camera;
	float m_dt;
	StateMachine m_stateMachine;
	bool m_isTransitioning{false};
	std::unordered_map<State, Page*> m_pages;
	TransitionGuard m_transitionGuard;
	HenkakuOptions m_henkakuOptions;
	ButtonEventFilter m_buttonFilter;

	// camera pan operations
	NumberAnimation m_cameraPanX;
	NumberAnimation m_cameraPanY;
};
