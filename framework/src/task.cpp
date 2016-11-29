/*
 * task.cpp - tree node for a task to be completed
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/task.h>

#include <numeric>

void Task::set(const TaskFunctor& functor)
{
	m_functor = functor;
}

void Task::set(const TaskFunctor&& functor)
{
	m_functor = functor;
}

void Task::insertDependant(const TaskPtr& task)
{
	m_subTasks.push_back(task);
}

void Task::insertDependant(TaskPtrList& tasks)
{
	m_subTasks.splice(m_subTasks.end(), tasks);
}

void Task::insertDependant(TaskPtrList&& tasks)
{
	m_subTasks.splice(m_subTasks.end(), tasks);
}

const TaskPtrList& Task::subTasks(void) const
{
	return m_subTasks;
}

size_t Task::count(void) const
{
	return std::accumulate(m_subTasks.begin(), m_subTasks.end(), 1LL, [](auto a, auto b)
	{
		return a + b->count();
	});
}

void Task::run(void) const
{
	if (m_functor)
		m_functor();
}
