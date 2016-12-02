/*
 * screen.cpp - abstract class for managing rendering screen
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/vitascreen.h>
#include <framework/guiapplication.h>
#include <framework/view.h>

VitaScreen::VitaScreen(void)
{
	// initialise graphics
}

VitaScreen::~VitaScreen(void)
{
}

void VitaScreen::draw(void)
{
	auto views = GuiApplication::allViews();
	
	for (auto view : views)
	{
		view->render();
	}
}

