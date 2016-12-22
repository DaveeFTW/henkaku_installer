
#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include "font.h"
#include "text.h"
#include "geometryrenderer.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

class GxmShaderPatcher;

class FpsCounter
{
public:
	FpsCounter(GxmShaderPatcher *patcher);

	void setModel(glm::mat4 model);
	glm::mat4 modelMatrix(void) const;

	glm::vec2 boundingBox(void) const;

	void update(float dt);
	void draw(SceGxmContext *ctx, const Camera *camera);

private:
	constexpr static const float smoothingRatio = 0.9;

private:
	Font m_font;
	Text m_fpsText;
	GeometryRenderer m_renderer;
	float m_fps{0.f};
};

#endif // FPSCOUNTER_H

