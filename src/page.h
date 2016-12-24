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

#include <glm/mat4x4.hpp>

struct SceGxmContext;
class Camera;
class ButtonEvent;

class Page
{
public:
	virtual ~Page(void) = default;

	void setModel(glm::mat4 model) { m_model = model; onModelChanged(model); }
	glm::mat4 modelMatrix(void) { return m_model; }

	virtual void update(float dt) { }
	virtual void draw(SceGxmContext *ctx, const Camera *camera) const = 0;

	virtual void onEvent(ButtonEvent *event) { }

private:
	virtual void onModelChanged(glm::mat4 model) = 0;

private:
	glm::mat4 m_model;
};

#endif // PAGE_H
