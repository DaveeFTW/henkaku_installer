/*
 * installerview.h - main UI view
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/view.h>
#include <framework/gxmshaderpatcher.h>

#include <stateless++/state_machine.hpp>

#include <memory>

class AnimatedBackground;
class Camera;
class FpsCounter;

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
	void update(void);

private:
	enum class Page
	{
		Init,
		Welcome,
		Setup
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

	using StateMachine = stateless::state_machine<Page, Trigger>;

private:
	TaskPtr m_simulationTasks;
	GxmShaderPatcher m_patcher;
	AnimatedBackground *m_animatedBackground;
	FpsCounter *m_fpsCounter;
	Camera *m_camera;
	float m_dt;
	StateMachine m_stateMachine;
};
