/*
 * buttoneventfilter.h - filter out repeating button events
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BUTTONEVENTFILTER_H
#define BUTTONEVENTFILTER_H

#include <framework/buttonevent.h>

class ButtonEventFilter
{
public:
	ButtonEventFilter(void);
	ButtonEvent filter(ButtonEvent *event);

private:
	unsigned int m_prevButtons;
};

#endif // BUTTONEVENTFILTER_H
