/*
 * viewmanager.cpp - handle control and events for views
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/viewmanager.h>
#include <framework/view.h>
#include <framework/elapsedtimer.h>
#include <framework/task.h>
#include <framework/taskscheduler.h>

#include <algorithm>

#include <psp2/display.h>

ViewManager::ViewManager(void)
	: m_taskScheduler(std::make_unique<TaskScheduler>())
{
}

ViewManager::~ViewManager(void)
{
}

void ViewManager::addView(ViewPtr view)
{
	m_views.push_back(view);
}

void ViewManager::removeView(ViewPtr view)
{
	m_views.erase(std::remove(m_views.begin(), m_views.end(), view), m_views.end());
}

void ViewManager::exec(void)
{
	ElapsedTimer timer;
	timer.start();

	while (false)
	{
		auto eventTasks = std::make_shared<Task>();
		m_taskScheduler->add(eventTasks);

		auto simulationTasks = std::make_shared<Task>();
		auto elapsed = timer.restart();

		// update simulation step at a consistent elapsed time quanta
		for (auto& view : m_views)
		{
			// update simulation
			simulationTasks->insertDependant(view->simulationTask(elapsed));
		}

		m_taskScheduler->add(simulationTasks);

		// add rendering tasks
		auto renderingTasks = std::make_shared<Task>();

		for (auto & view : m_views)
		{
			renderingTasks->insertDependant(view->renderingTask());
		}

		m_taskScheduler->add(renderingTasks);
	}
}
