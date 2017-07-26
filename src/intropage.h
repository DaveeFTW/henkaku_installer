/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef INTROPAGE_H
#define INTROPAGE_H

#include "page.h"
#include "rectangle.h"
#include "vertextypes.h"
#include "geometryrenderer.h"
#include "numberanimation.h"

#include <framework/gxmtexture.h>
#include <framework/gpumemoryblock.h>

#include <functional>

class IntroPage : public Page
{
public:
	IntroPage(GxmShaderPatcher *patcher);

	void update(float dt) final;
	void draw(SceGxmContext *ctx, const Camera *camera) const final;

	void onEvent(Event *event) final;

	// TODO: no more hacks please
	void setExitTrigger(std::function<void()> callback);

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);
	void loadTexture(GxmTexture *texture, const char *file);
	void fragmentTask(SceGxmContext *ctx);

private:
	GeometryRenderer m_renderer;
	Rectangle<ColouredGeometryVertex> m_rectangle;
	std::function<void()>  m_exitCallback;
	NumberAnimation m_fadeIn, m_pauseAnimation, m_fadeOut, m_fadeOutBg, m_fadeOutEnso;

	GeometryRenderer m_ensoRenderer;
	Rectangle<ColouredGeometryVertex> m_ensoRectangle;
	GxmTexture m_texture;
	GpuMemoryBlock<glm::vec2> m_texCoords;
};

#endif // INTROPAGE_H
