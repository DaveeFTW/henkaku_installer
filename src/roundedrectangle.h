/*
 * roundedrectangle.h - define the geometry of a rounded rectangle
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef ROUNDEDRECTANGLE_H
#define ROUNDEDRECTANGLE_H

#include "geometry.h"
#include "geometryrenderer.h"
#include "circularsegment.h"
#include "rectangle.h"

#include <framework/gpumemoryblock.h>

#include <glm/gtx/transform.hpp>

#include <memory>

template <typename Vertex>
class RoundedRectangle : public Geometry
{
	static_assert(Vertex::streamCount() == 1, "cannot create RoundedRectangle with multiple streams!");
public:
	RoundedRectangle(float width, float height, float radius);

	template <typename Colour>
	void setColour(Colour colour);

	float width(void) const { return m_rectVertical.width(); }
	float height(void) const { return m_rectHorizontal.height(); }
	float radius(void) const { return m_bl.radius(); }

private:
	void updateWorldMatrices(glm::mat4 matrix);
	void onModelChanged(glm::mat4 model) override;
	void doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const override;

private:
	mutable CircularSegment<Vertex> m_bl, m_br, m_tl, m_tr;
	mutable Rectangle<Vertex> m_rectVertical, m_rectHorizontal;
};

template <typename Vertex>
RoundedRectangle<Vertex>::RoundedRectangle(float width, float height, float radius)
	: m_bl(radius, 95.f)
	, m_br(radius, 95.f)
	, m_tl(radius, 95.f)
	, m_tr(radius, 95.f)
	, m_rectVertical(width, height+radius*2)
	, m_rectHorizontal(width+radius*2, height)
{
	m_tr.setTranslation(width, height);
	m_tr.setRotation(0.f);
	m_tl.setTranslation(0, height);
	m_tl.setRotation(90.f);
	m_bl.setTranslation(0, 0);
	m_bl.setRotation(180.f);
	m_br.setTranslation(width, 0);
	m_br.setRotation(270.f);

	m_rectVertical.setTranslation(radius, 0);
	m_rectHorizontal.setTranslation(0, radius);

	updateWorldMatrices(modelMatrix());
}

template <typename Vertex>
template <typename Colour>
void RoundedRectangle<Vertex>::setColour(Colour colour)
{
	m_bl.setColour(colour);
	m_br.setColour(colour);
	m_tl.setColour(colour);
	m_tr.setColour(colour);
	m_rectVertical.setColour(colour);
	m_rectHorizontal.setColour(colour);
}

template <typename Vertex>
void RoundedRectangle<Vertex>::updateWorldMatrices(glm::mat4 matrix)
{
	m_tr.setWorldMatrix(matrix);
	m_tl.setWorldMatrix(matrix);
	m_bl.setWorldMatrix(matrix);
	m_br.setWorldMatrix(matrix);
	m_rectVertical.setWorldMatrix(matrix);
	m_rectHorizontal.setWorldMatrix(matrix);
}

template <typename Vertex>
void RoundedRectangle<Vertex>::onModelChanged(glm::mat4 model)
{
	updateWorldMatrices(model);
}

template <typename Vertex>
void RoundedRectangle<Vertex>::doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const
{
	// prevent overlapping by stenciling
	sceGxmSetFrontStencilRef(ctx, 1);
	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_ALWAYS, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_REPLACE, 1, ~0);

	renderer->draw(ctx, camera, &m_bl);
	renderer->draw(ctx, camera, &m_br);
	renderer->draw(ctx, camera, &m_tl);
	renderer->draw(ctx, camera, &m_tr);

	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_NOT_EQUAL, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_REPLACE, 1, ~0);

	renderer->draw(ctx, camera, &m_rectVertical);
	renderer->draw(ctx, camera, &m_rectHorizontal);

	sceGxmSetFrontStencilRef(ctx, 0);
	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_ALWAYS, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, 0, 0);
}

#endif // ROUNDEDRECTANGLE_H
