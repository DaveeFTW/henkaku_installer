/*
 * page.h - baseclass for a page
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef PAGE_H
#define PAGE_H

#include "worldentity.h"

struct SceGxmContext;
class Camera;
class ButtonEvent;

class Page : public WorldEntity
{
public:
	virtual ~Page(void) = default;

	virtual void update(float dt) { }
	virtual void draw(SceGxmContext *ctx, const Camera *camera) const = 0;

	virtual void onEvent(ButtonEvent *event) { }
};

#endif // PAGE_H
