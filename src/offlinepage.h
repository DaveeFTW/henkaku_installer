/*
 * offlinepage.h - let the user decide if they wish to use the henkaku offline enable
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef OFFLINEPAGE_H
#define OFFLINEPAGE_H

#include "page.h"
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "checkbox.h"

class OfflinePage : public Page
{
public:
	OfflinePage(GxmShaderPatcher *patcher);

	bool installOffline(void) const;

	void update(float dt);
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

#endif // OFFLINEPAGE_H
