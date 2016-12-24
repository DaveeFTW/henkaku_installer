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

#include <stateless++/state_machine.hpp>

class ResetPage : public Page
{
public:
	ResetPage(GxmShaderPatcher *patcher);

	bool reset(void) const;

	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(ButtonEvent *event) final;

private:
	enum class Trigger
	{
		Cross,
	};

	using StateMachine = stateless::state_machine<bool, Trigger>;

private:
	void onModelChanged(glm::mat4 model) final;

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12;
	Text m_welcomeText, m_nextPageDirection;
	StateMachine m_stateMachine;
};

#endif // RESETPAGE_H
