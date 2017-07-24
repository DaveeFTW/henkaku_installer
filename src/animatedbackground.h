/*
 * animatedbackground.h - display and control animated background surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
 
#ifndef ANIMATEDBACKGROUND_H
#define ANIMATEDBACKGROUND_H

#include <framework/gxmtexture.h>
#include <framework/gpumemoryblock.h>

#include "geometryrenderer.h"
#include "rectangle.h"
#include "vertextypes.h"

struct SceGxmContext;
class Camera;

class AnimatedBackground
{
public:
	AnimatedBackground(GxmShaderPatcher *patcher);

	void update(float dt);
	void draw(SceGxmContext *ctx, const Camera *camera);

	void setColour(glm::vec4 topleft, glm::vec4 bottomRight);
	glm::vec4 topLeftColour(void) const;
	glm::vec4 bottomRightColour(void) const;

	void setDisplacementModifier(int texId, float dx, float dy);
	glm::vec2 displacementRate(int texId) const;

private:
	struct TextureCoordVertex
	{
		glm::vec2 texCoord[5];
	};

	struct BgTexture
	{
		GxmTexture texture;
		glm::vec2 position;
		glm::vec2 dispRate;
		glm::vec2 dispModifier;
	};

private:
	void loadTexture(GxmTexture *texture, const char *file);
	void fragmentTask(SceGxmContext *ctx);

private:
	GeometryRenderer m_renderer;
	Rectangle<ColouredGeometryVertex> m_rectangle;
	
	BgTexture m_textures[5];
	std::unique_ptr<GpuMemoryBlock<TextureCoordVertex>> m_texCoords;

	glm::vec4 m_bottomRightColour, m_topLeftColour;
};

#endif // ANIMATEDBACKGROUND_H
