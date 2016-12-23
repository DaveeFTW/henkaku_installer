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
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"

class WelcomePage : public Page
{
public:
	WelcomePage(GxmShaderPatcher *patcher);

	void setModel(glm::mat4 model) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12;
	Text m_welcomeText, m_nextPageDirection;
};

#endif // WELCOMEPAGE_H
