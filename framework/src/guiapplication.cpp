/*
 * guiapplication.cpp - provide event and gui handling for application
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/guiapplication.h>
#include <framework/taskscheduler.h>
#include <framework/view.h>
#include <framework/task.h>
#include <framework/vitainput.h>
#include <framework/elapsedtimer.h>
#include <framework/vitascreen.h>

#include <iostream>

#include <psp2/display.h>

GuiApplication *GuiApplication::self = nullptr;

GuiApplication::GuiApplication(int argc, char **argv)
	: platform_screen(nullptr)
	, platform_input(nullptr)
	, focused_view(nullptr)
{
	if (self)
	{
		std::cerr << __func__ << ": attempting to instantiate second GuiApplication object." << std::endl;
		std::abort();
	}

	self = this;

	// create a screen for the platform
	// in this case we only have the vita as a platform
	platform_screen = new VitaScreen();

	// create an input source
	platform_input = new VitaInput();

	update_task = std::make_shared<Task>();

	for (auto& task : platform_input->tasks())
	{
		update_task->insertDependant(task);
	}
	
	
	draw_task = std::make_shared<Task>();
	//draw_task->set([this](void)
	//{
		//this->platform_screen->draw();
	//});
	draw_task->set(std::bind(&Screen::draw, platform_screen));
}

GuiApplication::~GuiApplication(void)
{
	self = nullptr;

	// destroy our screen
	//delete platform_screen;
	platform_screen = nullptr;

	// destroy our input
	//delete platform_input;
	platform_input = nullptr;

	focused_view = nullptr;
	view_list.clear();
}

int GuiApplication::exec(void)
{
	if (!self)
	{
		std::cerr << __func__ << ": Application object not instantiated." << std::endl;
		return -1;
	}

	std::mutex mutex;
	std::condition_variable cv;
	
	TaskScheduler scheduler;
	ElapsedTimer timer;
	timer.start();
	bool complete = false;
	
	auto resume_task = std::make_shared<Task>();
	resume_task->set([&complete, &mutex, &cv](void)
	{
		// sync this to vblank
		sceDisplayWaitVblankStart();
		
		{
			std::unique_lock<std::mutex> lock(mutex);
			complete = true;
		}
		
		cv.notify_one();
	});
	
	while (true)
	{
		complete = false;
		
		// all registered event sources will be called to send events (if desired)
		scheduler.add(self->update_task);

		// update simulation step for focused view
		auto focused_view = GuiApplication::focusedView();

		for (auto& view : self->view_list)
			scheduler.add(view->simulationTask(timer.restart()));

		scheduler.add(self->draw_task);
		scheduler.add(resume_task);

		// we wait until ready
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [&complete]{ return complete; });
	}

	return 0;
}

ViewPtr GuiApplication::focusedView(void)
{
	if (!self)
	{
		std::cerr << __func__ << ": Application object not instantiated." << std::endl;
		return nullptr;
	}

	return self->focused_view;
}

ViewPtrList GuiApplication::allViews(void)
{
	if (!self)
	{
		std::cerr << __func__ << ": Application object not instantiated." << std::endl;
		return {};
	}

	return self->view_list;
}

void GuiApplication::addView(ViewPtr view)
{
	if (!self)
	{
		std::cerr << __func__ << ": Application object not instantiated." << std::endl;
		return;
	}

	self->view_list.push_back(view);
	//screen->onViewAdded(view);
}
