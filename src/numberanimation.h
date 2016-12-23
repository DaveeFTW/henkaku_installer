/*
 * numberanimation.h - animate a number over a specified period of time
 *
 * Copyright (C) 2015 - 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef NUMBERANIMATION_H
#define NUMBERANIMATION_H

#include <functional>
#include <vector>

class NumberAnimation
{
public:
	using StepHandler = std::function<void(float num)>;
	using CompletionHandler = std::function<void(void)>;
	using EasingTransform = std::function<float(float, float, float, float)>;
	
public:
	NumberAnimation(float start=0, float end=1, int duration=200);

	void setEasing(EasingTransform easing);

	void setStart(float start);
	float start(void) const;

	void setEnd(float end);
	float end(void) const;

	void setDuration(int duration);
	int duration(void) const;

	void start(void);
	void update(float dt);
	bool complete(void) const;
	
	void setStepHandler(StepHandler handler);
	void setCompletionHandler(CompletionHandler handler);

private:
	float m_start, m_end, m_elapsed;
	int m_duration;
	bool m_complete{false};

	StepHandler m_stepHandler;
	CompletionHandler m_completionHandler;
	EasingTransform m_easing;
};

#endif // ANIMATION_H