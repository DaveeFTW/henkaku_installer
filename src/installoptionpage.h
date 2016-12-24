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
#include "roundedrectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "text.h"
#include "font.h"

#include <stateless++/state_machine.hpp>

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
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

	void onEvent(ButtonEvent *event) final;

private:
	enum class Trigger
	{
		Up,
		Down
	};

	using StateMachine = stateless::state_machine<Selection, Trigger>;

private:
	void onModelChanged(glm::mat4 model) final;

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12;
	Text m_welcomeText, m_nextPageDirection;
	StateMachine m_stateMachine;
};

#endif // INSTALLOPTIONPAGE_H
