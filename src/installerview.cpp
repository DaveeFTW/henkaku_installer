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

#include <framework/task.h>

#include <glm/glm.hpp>

#include <easyloggingpp/easylogging++.h>

InstallerView::InstallerView(void)
{
	m_animatedBackground = new AnimatedBackground(&m_patcher);
	m_camera = new Camera;

	// align z = 0 to screen coordiantes
	auto fov = 45.f;
	m_camera->setPerspectiveProjection(glm::radians(fov), 960.f/544.f, 0.1f, 1024.f);
	float angle1=fov/2.0;
	float angle2=180 - (90 + angle1);
	float Z = 0.5 * 544.f * std::sin(glm::radians(angle2))/std::sin(glm::radians(angle1));
	m_camera->setPosition(glm::vec3(960.f/2, 544.f/2, Z));
	m_camera->setViewCenter(glm::vec3(960.f/2, 544.f/2, 0));
	m_camera->setUpVector(glm::vec3(0, 1, 0));

	m_simulationTasks = std::make_shared<Task>();
	m_simulationTasks->set(std::bind(&InstallerView::update, this));

	auto animatedBackgroundTask = std::make_shared<Task>();
	animatedBackgroundTask->set(std::bind(&AnimatedBackground::update, m_animatedBackground, std::cref(m_dt)));

	// add as dependent tasks
	m_simulationTasks->insertDependant(animatedBackgroundTask);
}

InstallerView::~InstallerView(void)
{
	delete m_camera;
	delete m_animatedBackground;
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
}
