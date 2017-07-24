/*
 * successpage.h - display installation success page
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef SUCCESSPAGE_H
#define SUCCESSPAGE_H

#include "page.h"
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"

class SuccessPage : public Page
{
public:
	SuccessPage(GxmShaderPatcher *patcher);

	void draw(SceGxmContext *ctx, const Camera *camera) const final;

private:
	void onModelChanged(glm::mat4 model) final;

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12;
	Text m_welcomeText, m_nextPageDirection;
};

#endif // SUCCESSPAGE_H
