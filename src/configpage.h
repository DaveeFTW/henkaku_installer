/*
 * configpage.h - let the user decide which features they would like enabled or disabled
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include "page.h"
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"
#include "checkboxmenu.h"
#include "checkbox.h"

class ConfigPage : public Page
{
	enum class UnsafeHomebrew : bool
	{
		Enabled,
		Disabled
	};

	enum class VersionSpoofing : bool
	{
		Enabled,
		Disabled
	};

public:
	ConfigPage(GxmShaderPatcher *patcher);

	UnsafeHomebrew unsafeHomebrew(void) const;
	VersionSpoofing versionSpoofing(void) const;

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(ButtonEvent *event) final;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font10, m_font8;
	Text m_titleText, m_nextPageDirection, m_unsafeLabel, m_unsafeLabelDesc, m_spoofLabel, m_spoofLabelDesc;
	CheckBoxMenu m_menu;
	CheckBox m_unsafeCheckbox, m_spoofCheckbox;
};

#endif // CONFIGPAGE_H
