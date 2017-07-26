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
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "menu.h"
#include "numberanimation.h"

#include <unordered_map>

class InstallOptionPage : public Page
{
public:
	enum class Selection
	{
		Install,
		Reactivate,
		Uninstall,
		Exit
	};

public:
	InstallOptionPage(GxmShaderPatcher *patcher, bool ensoInstalled=false);

	Selection selection(void) const;

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

	void onEvent(Event *event) final;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font12, m_font8;
	Text m_nextPageDirection;
	Text m_installText, m_installDesc, m_reactivateText, m_reactivateDesc, m_uninstallText, m_uninstallDesc, m_exitText, m_exitDesc;
	Menu m_menu;
	std::unordered_map<int, Selection> m_selectionMap;
	NumberAnimation m_fadeInMenu, m_fadeInDirection;
};

#endif // INSTALLOPTIONPAGE_H
