/*
 * buttoneventfilter.cpp - filter out repeating button events
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "buttoneventfilter.h"

ButtonEventFilter::ButtonEventFilter(void)
	: m_prevButtons(0)
{
}

ButtonEvent ButtonEventFilter::filter(ButtonEvent *event)
{
	ButtonEvent filter(event->buttons() & ~m_prevButtons);
	m_prevButtons = event->buttons();
	return filter;
}
