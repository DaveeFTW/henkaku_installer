/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef WARNINGPAGE_H
#define WARNINGPAGE_H

#include "page.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "numberanimation.h"

class WarningPage : public Page
{
public:
	WarningPage(GxmShaderPatcher *patcher);

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(Event *event) final;

	// TODO: no more hacks please
	void setExitTrigger(std::function<void()> callback);

	bool transitionGuard() const override;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	GeometryRenderer m_textRenderer;
	Font m_font20, m_font12, m_font8;
	Text m_titleText, m_description, m_description2, m_description3, m_description4, m_description5, m_description6, m_nextPageDirection;
	std::function<void()> m_exitCallback;
	NumberAnimation m_fadeInTitle, m_fadeInText, m_fadeInDirection;
	bool m_transitionGuard;
};

#endif // WARNINGPAGE_H
