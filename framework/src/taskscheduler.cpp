/*
 * taskscheduler.cpp - effectively apply tasks to a threadpool
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/taskscheduler.h>

#include <ctpl/ctpl_stl.h>

TaskScheduler::TaskScheduler(void)
	: m_threadPool(std::make_unique<ctpl::thread_pool>(MAX_THREADS))
	, m_running(true)
{
	m_schedulerThread = std::thread([this](void)
	{
		this->waitForTasks();
	});
}

TaskScheduler::~TaskScheduler(void)
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_running = false;
		m_cv.notify_one();
	}

	m_schedulerThread.join();
}

void TaskScheduler::add(TaskPtr task)
{
	auto info = std::make_shared<TaskInfo>();

	info->started = false;
	info->remainingUnits = task->count();
	info->task = task;

	std::unique_lock<std::mutex> lock(m_mutex);
	m_taskQueue.push(std::move(info));
	m_cv.notify_one();
}

void TaskScheduler::waitForTasks(void)
{
	while (m_running)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		if (!m_running)
			break;

		m_cv.wait(lock, [this](void)
		{
			if (!this->m_running)
				return true;

			if (this->m_taskQueue.empty())
				return false;

			if (!this->m_taskQueue.front()->started)
				return true;

			if (this->m_taskQueue.front()->remainingUnits)
				return false;

			this->m_taskQueue.pop();
			return !this->m_taskQueue.empty();
		});

		if (!m_running)
			break;

		auto task = m_taskQueue.front();

		task->started = true;
		schedule(task);
	}
}

void TaskScheduler::queueTask(TaskInfoPtr taskInfo, TaskPtr task)
{
	m_threadPool->push([this, taskInfo, task](auto id)
	{
		task->run();

		{
			std::unique_lock<std::mutex> lock(this->m_mutex);
			--taskInfo->remainingUnits;
			this->m_cv.notify_one();
		}

		this->scheduleSubtasks(taskInfo, task);
	});
}

void TaskScheduler::scheduleSubtasks(TaskInfoPtr taskInfo, TaskPtr task)
{
	for (auto &subtask : task->subTasks())
	{
		queueTask(taskInfo, subtask);
	}
}

void TaskScheduler::schedule(TaskInfoPtr task)
{
	queueTask(task, task->task);
}
