/*
 * installoptionpage.h - let the user decide between simple or advanced install
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef INSTALLOPTIONPAGE_H
#define INSTALLOPTIONPAGE_H

#include "page.h"
#include "rectangle.h"
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "menu.h"

#include <unordered_map>

class InstallOptionPage : public Page
{
public:
	enum class Selection
	{
		Simple,
		Custom
	};

public:
	InstallOptionPage(GxmShaderPatcher *patcher);

	Selection selection(void) const;

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

	void onEvent(ButtonEvent *event) final;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	Rectangle<ColouredGeometryVertex> m_selectionBox;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font18, m_font16, m_font8;
	Text m_titleText, m_simpleInstallationText, m_simpleInstallationDesc, m_customInstallationText, m_customInstallationDesc, m_nextPageDirection;
	Menu m_menu;
	std::unordered_map<int, Selection> m_selectionMap;
};

#endif // INSTALLOPTIONPAGE_H
