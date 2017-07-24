/*
 * buttonevent.h - describe a button input event
 *
 * Copyright (C) 2015-2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BUTTONEVENT_H
#define BUTTONEVENT_H

#include <framework/event.h>

class ButtonEvent : public Event
{
public:
	ButtonEvent(unsigned int buttons) : Event(Event::Button) { m_buttons = buttons; }
	unsigned int buttons(void) const { return m_buttons; }

private:
	unsigned int m_buttons;
};

#endif // BUTTONEVENT_H
