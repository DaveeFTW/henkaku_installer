/*
 * taskscheduler.h - effectively apply tasks to a threadpool
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include <framework/task.h>

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace ctpl
{
	class thread_pool;
} // namespace ctpl

class TaskScheduler
{
	const int MAX_THREADS = 5;

public:
	TaskScheduler(void);
	~TaskScheduler(void);

	void add(TaskPtr task);

private:
	using TaskInfo = struct
	{
		bool started;
		size_t remainingUnits;
		TaskPtr task;
	};

	using TaskInfoPtr = std::shared_ptr<TaskInfo>;

	void waitForTasks(void);
	void queueTask(TaskInfoPtr taskInfo, TaskPtr task);
	void scheduleSubtasks(TaskInfoPtr taskInfo, TaskPtr task);
	void schedule(TaskInfoPtr task);

private:
	std::unique_ptr<ctpl::thread_pool> m_threadPool;
	std::queue<TaskInfoPtr> m_taskQueue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::thread m_schedulerThread;
	bool m_running;
};

#endif // TASKSCHEDULER_H

