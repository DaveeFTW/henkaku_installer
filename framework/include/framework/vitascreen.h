/*
 * vitascreen.h - 
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VITASCREEN_H
#define VITASCREEN_H

#include <framework/screen.h>

class VitaScreen : public Screen
{
public:
	VitaScreen(void);
	~VitaScreen(void);

	void draw(void) override;
};

#endif // VITASCREEN_H
