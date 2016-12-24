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

#include <stateless++/state_machine.hpp>

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

	void draw(SceGxmContext *ctx, const Camera *camera) const final;
	void onEvent(ButtonEvent *event) final;

private:
	enum class State
	{
		UnsafeHomebrew,
		VersionSpoofing
	};

	enum class Trigger
	{
		Up,
		Down,
	};
	
	enum class CheckboxTrigger
	{
		Cross
	};

	using StateMachine = stateless::state_machine<State, Trigger>;
	
	template <typename T>
	using Checkbox = stateless::state_machine<T, CheckboxTrigger>;

private:
	void onModelChanged(glm::mat4 model) final;

private:
	RoundedRectangle<ColouredGeometryVertex> m_rectangle;
	GeometryRenderer m_renderer, m_textRenderer;
	Font m_font20, m_font12;
	Text m_welcomeText, m_nextPageDirection;
	StateMachine m_stateMachine;
	Checkbox<UnsafeHomebrew> m_unsafeCheckbox;
	Checkbox<VersionSpoofing> m_spoofCheckbox;
};

#endif // CONFIGPAGE_H
