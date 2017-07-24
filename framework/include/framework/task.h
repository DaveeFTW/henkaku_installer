/*
 * task.h - tree node for a task to be completed
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef TASK_H
#define TASK_H

#include <list>
#include <functional>
#include <memory>

class Task;
using TaskList = std::list<Task>;
using TaskFunctor = std::function<void()>;
using TaskPtr = std::shared_ptr<Task>;
using TaskPtrList = std::list<TaskPtr>;

class Task
{
public:
	void set(const TaskFunctor& functor);
	void set(const TaskFunctor&& functor);

	void insertDependant(const TaskPtr& task);
	void insertDependant(TaskPtrList& tasks);
	void insertDependant(TaskPtrList&& tasks);

	const TaskPtrList& subTasks(void) const;

	size_t count(void) const;

	void run(void) const;

private:
	TaskPtrList m_subTasks;
	TaskFunctor m_functor;
};

#endif // TASK_H
