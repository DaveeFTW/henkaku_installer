/*
 * elapsedtimer.cpp - provide a easy way to calculate elapsed timese
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#include <psp2/rtc.h>

class ElapsedTimer
{
public:
	ElapsedTimer(void);
	ElapsedTimer &operator=(const ElapsedTimer &rhs) = delete;
	ElapsedTimer(const ElapsedTimer &rhs) = delete;

	void start(void);
	double restart(void);
	double elapsed(void) const;

private:
	double m_tickResolution;
	SceRtcTick m_time;
};

#endif // ELAPSEDTIMER_H
