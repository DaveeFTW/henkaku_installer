/*
 * Enso installer
 * Copyright (C) 2017 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef STATICBACKGROUND_H
#define STATICBACKGROUND_H

#include <framework/gxmtexture.h>
#include <framework/gpumemoryblock.h>

#include "geometryrenderer.h"
#include "rectangle.h"
#include "vertextypes.h"

struct SceGxmContext;
class Camera;

class StaticBackground
{
public:
	StaticBackground(GxmShaderPatcher *patcher);

	void update(float dt);
	void draw(SceGxmContext *ctx, const Camera *camera);

	void setColour(glm::vec4 topLeft, glm::vec4 bottomRight);

private:
	void loadTexture(GxmTexture *texture, const char *file);
	void fragmentTask(SceGxmContext *ctx);

private:
	GeometryRenderer m_renderer;
	Rectangle<ColouredGeometryVertex> m_rectangle;

	GxmTexture m_texture;
	GpuMemoryBlock<glm::vec2> m_texCoords;

	glm::vec4 m_bottomRightColour, m_topLeftColour;
};

#endif // STATICBACKGROUND_H
