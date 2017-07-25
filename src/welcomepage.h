/*
 * welcomepage.h - the first page the user is presented
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include "page.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "numberanimation.h"

#include <functional>

class WelcomePage : public Page
{
public:
	WelcomePage(GxmShaderPatcher *patcher);

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

	void fadeIn();
	void fadeOut();

	// TODO: no more hacks please
	void setExitTrigger(std::function<void()> callback);

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	GeometryRenderer m_textRenderer;
	Font m_font28;
	Text m_welcomeText;
	std::function<void()>  m_exitCallback;
	NumberAnimation m_fadeIn, m_pauseAnimation, m_fadeOut;
};

#endif // WELCOMEPAGE_H
