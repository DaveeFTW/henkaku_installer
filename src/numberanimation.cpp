/*
 * numberanimation.cpp - animate a number over a specified period of time
 *
 * Copyright (C) 2015 - 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "numberanimation.h"

NumberAnimation::NumberAnimation(float start, float end, int duration)
	: m_start(start)
	, m_end(end)
	, m_duration(duration)
	, m_stepHandler(nullptr)
	, m_completionHandler(nullptr)
{
	// default to linear transform
	m_easing = [](float t, float b, float c, float d)
	{
		return c*t/d + b;
	};
}

void NumberAnimation::setEasing(EasingTransform easing)
{
	m_easing = easing;
}

void NumberAnimation::setStart(float start)
{
	m_start = start;
}

float NumberAnimation::getStart(void) const
{
	return m_start;
}

void NumberAnimation::setEnd(float end)
{
	m_end = end;
}

float NumberAnimation::end(void) const
{
	return m_end;
}

void NumberAnimation::setDuration(int duration)
{
	m_duration = duration;
}

int NumberAnimation::duration(void) const
{
	return m_duration;
}

void NumberAnimation::start(void)
{
	m_elapsed = 0.f;
	m_complete = false;
}

void NumberAnimation::setStepHandler(StepHandler handler)
{
	m_stepHandler = handler;
}

void NumberAnimation::setCompletionHandler(CompletionHandler handler)
{
	m_completionHandler = handler;
}

void NumberAnimation::update(float dt)
{
	if (complete())
		return;

	if (m_elapsed >= m_duration/1000.f)
	{
		m_complete = true;

		// specify the precise end value instead
		if (m_stepHandler)
			m_stepHandler(m_start + m_end);

		if (m_completionHandler)
			m_completionHandler();
	}
	else if (m_stepHandler)
	{
		// calculate number
		float number = m_easing(m_elapsed, m_start, m_end, m_duration/1000.f);
		m_stepHandler(number);
	}

	m_elapsed += dt;
}

bool NumberAnimation::complete(void) const
{
	return m_complete;
}
