/*
 * resetpage.h - let the user decide if they wish to reset their henkaku config
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef RESETPAGE_H
#define RESETPAGE_H

#include "page.h"
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "checkbox.h"

class ResetPage : public Page
{
public:
	ResetPage(GxmShaderPatcher *patcher);

	bool reset(void) const;

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(ButtonEvent *event) final;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12, m_font8;
	Text m_titleText, m_description, m_description2, m_description3, m_nextPageDirection, m_checkBoxLabel;
	CheckBox m_checkbox;
};

#endif // RESETPAGE_H
