/*
 * event.h - describe the base of a simple event
 *
 * Copyright (C) 2015-2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
	enum Type
	{
		None,
		Button,
		FocusIn
	};

public:
	Event(Type type) { m_type = type; }
	virtual ~Event(void) = default;

	Type type(void) const { return m_type; }

private:
	Type m_type;
};

#endif // EVENT_H
