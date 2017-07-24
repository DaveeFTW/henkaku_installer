/*
 * screen.h - abstract class for managing rendering screen
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <memory>

class View;
using ViewPtr = std::shared_ptr<View>;

class Screen
{
public:
	virtual void draw(void) = 0;
	//virtual void onViewAdded(ViewPtr view) = 0;
	
private:
};

#endif // SCREEN_H
