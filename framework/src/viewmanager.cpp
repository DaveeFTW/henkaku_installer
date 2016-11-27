/*
 * viewmanager.cpp - handle control and events for views
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/viewmanager.h>
#include <framework/elapsedtimer.h>

#include <algorithm>

#include <psp2/display.h>

ViewManager::ViewManager(void)
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
	
	while (true)
	{
		sceDisplayWaitVblankStart();
	}
}

/*










//####################### PSP BELOW
#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>
#include <pspdisplay.h>

#include <algorithm>

//#include "vertextype.h"
//#include <pspdebug.h>
//#include <string.h>

namespace {
    void sceGumOrtho2(float left, float right, float bottom, float top, float near, float far)
    {
    	ScePspFMatrix4 *t;
        ScePspFMatrix4 t2;
        t = &t2;
    	float dx = right-left, dy = top-bottom, dz = far-near;

    	memset(t,0,sizeof(ScePspFMatrix4));

    	t->x.x = 2.0f / dx;
    	t->w.x = -(right + left) / dx;
    	t->y.y = 2.0f / dy;
    	t->w.y = -(top + bottom) / dy;
    	t->z.z = -2.0f / dz;
    	t->w.z = -(far + near) / dz;
    	t->w.w = 1.0f;

    	sceGumMultMatrix(t);
    }
}

ViewManager::ViewManager(void)
    : m_graphicsDevice(new GraphicsDevice())
    , m_buttonArbiter(new ButtonArbiter())
    , m_stateMachine(nullptr)
    , m_scene(nullptr)
{
}

void ViewManager::addView(View *view)
{
    m_views.push_back(view);
}

void ViewManager::removeView(View *view)
{
    auto it = std::find(m_views.begin(), m_views.end(), view);

    if (it != m_views.end())
    {
        m_views.erase(it);
    }
}

void ViewManager::setStateMachine(StateMachine *machine)
{
    m_stateMachine = machine;
}

GraphicsDevice *ViewManager::graphics(void)
{
    return m_graphicsDevice;
}

void ViewManager::exec(void)
{
    m_graphicsDevice->initialise();

    Timer *timer = Timer::instance();
    timer->start();

    while (true)
    {
        // process all events
        Application::instance()->processEvents();

        sceGuStart(GU_DIRECT, m_graphicsDevice->displayList());
        sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

        // disable depth test
        sceGuDepthMask(GU_TRUE);

        // setup ortho projection
        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho2(0, 480, 272, 0, -1, 1);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();

        auto elapsedTime = timer->elapsed(true);

        for (auto it = m_views.begin(); it != m_views.end(); ++it)
        {
            // update our scene
            (*it)->update(elapsedTime);

            sceGumMatrixMode(GU_MODEL);
            sceGumLoadMatrix((*it)->model());
            sceGumUpdateMatrix();

            (*it)->render();
        }

        sceGuFinish();
        sceGuSync(0,0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
}
*/
