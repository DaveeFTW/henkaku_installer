/*
 * elapsedtimer.cpp - provide a easy way to calculate elapsed times
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/elapsedtimer.h>

ElapsedTimer::ElapsedTimer(void)
{
	m_tickResolution = static_cast<double>(sceRtcGetTickResolution());
}

void ElapsedTimer::start(void)
{
	sceRtcGetCurrentTick(&m_time);
}

double ElapsedTimer::elapsed(void) const
{
	SceRtcTick curTime;
	sceRtcGetCurrentTick(&curTime);

	auto elapsedTicks = curTime.tick - m_time.tick;
	return static_cast<double>(elapsedTicks)/m_tickResolution;
}

double ElapsedTimer::restart(void)
{
	SceRtcTick curTime;
	sceRtcGetCurrentTick(&curTime);

	auto elapsedTicks = curTime.tick - m_time.tick;
	m_time = curTime;

	return static_cast<double>(elapsedTicks)/m_tickResolution;
}
