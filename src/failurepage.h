/*
 * failurepage.h - display installation failure information
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef FAILUREPAGE_H
#define FAILUREPAGE_H

#include "page.h"
#include "rectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "numberanimation.h"

class FailurePage : public Page
{
public:
	FailurePage(GxmShaderPatcher *patcher);

	void update(float dt) override;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(Event *event) override;

	void setMessage(const std::string& msg);
	void setMessage(const std::vector<std::string>& msg);
	void setMessage(std::initializer_list<std::string> msg);

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents();

private:
	Rectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12, m_font8;
	Text m_titleText, m_nextPageDirection;
	std::vector<Text> m_message;
	NumberAnimation m_fadeInError, m_fadeInTitle, m_fadeInMessage, m_fadeInDirection;
};

#endif // FAILUREPAGE_H
