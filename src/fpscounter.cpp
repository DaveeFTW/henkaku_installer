/*
 * fpscounter.cpp - simple fps counter
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "fpscounter.h"

#include <sstream>
#include <iomanip>

FpsCounter::FpsCounter(GxmShaderPatcher *patcher)
	: m_font("rsc:/fonts/DroidSans.ttf")
	, m_fpsText(&m_font, "FPS: 00.00")
	, m_renderer(patcher)
{
	m_renderer.setShaders<ColouredTextureVertex>("rsc:/backgroundtext.vert.cg.gxp", "rsc:/backgroundtext.frag.cg.gxp");
}

void FpsCounter::setModel(glm::mat4 model)
{
	//m_fpsText.setModel(model);
}

glm::mat4 FpsCounter::modelMatrix(void) const
{
	return m_fpsText.modelMatrix();
}

glm::vec2 FpsCounter::boundingBox(void) const
{
	return m_fpsText.boundingBox();
}

void FpsCounter::update(float dt)
{
	m_fps = (m_fps * smoothingRatio) + (dt * (1.f - smoothingRatio));

	std::stringstream stream;
	stream << "FPS: " << std::fixed << std::setprecision(2) << (1.f/m_fps);
	m_fpsText.setText(stream.str());
	m_fpsText.setColour(glm::vec4(0.f, 0.f, 0.f, 1.f));
}

void FpsCounter::draw(SceGxmContext *ctx, const Camera *camera)
{
	m_renderer.draw(ctx, camera, &m_fpsText);
}
