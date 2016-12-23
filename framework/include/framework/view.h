/*
 * view.h - base class for a view surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VIEW_H
#define VIEW_H

#include <memory>

class Task;
using TaskPtr = std::shared_ptr<Task>;

class Event;

struct SceGxmContext;

class View
{
	friend class ViewManager;
	friend class Screen;
	friend class GuiApplication;

public:
	View(void);
	virtual ~View(void) = default;


	virtual TaskPtr simulationTask(double dt) = 0;
	//virtual TaskPtr renderingTask(void) = 0;
	
	//virtual void setOpacity(float opacity) = 0;
	//virtual float opacity(void) = 0;
	void show(void);
	
	virtual void render(SceGxmContext *ctx) = 0;

protected:
	virtual void onEvent(Event *event) { }
	
private:
	//http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.htm
};

#endif // VIEW_H