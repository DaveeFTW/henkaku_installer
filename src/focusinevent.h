/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef FOCUSINEVENT_H
#define FOCUSINEVENT_H

#include <framework/event.h>

class FocusInEvent : public Event
{
public:
	FocusInEvent() : Event(Event::FocusIn) { }
};

#endif // FOCUSINEVENT_H
