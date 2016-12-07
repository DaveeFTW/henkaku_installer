/*
 * installerview.cpp - main UI view
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "installerview.h"
#include <framework/task.h>

InstallerView::InstallerView(void)
{
	m_simulationTasks = std::make_shared<Task>();
}

TaskPtr InstallerView::simulationTask(double dt)
{
	return m_simulationTasks;
}

void InstallerView::render(SceGxmContext *ctx)
{

}
