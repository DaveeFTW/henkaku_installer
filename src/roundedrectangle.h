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

private:
	glm::mat4 rotateTranslate(float deg, float radius, float tx, float ty);
	void doDraw(SceGxmContext *ctx, GeometryRenderer *renderer, const Camera *camera) const override;

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
	, m_rectVertical(0, 0, 0, width, height+radius*2)
	, m_rectHorizontal(0, 0, 0, width+radius*2, height)
{
	m_tr.setModel(glm::translate(glm::mat4(1), glm::vec3(width, height, 0)));
	m_tl.setModel(rotateTranslate(90.f, radius, 0, height));
	m_bl.setModel(rotateTranslate(180.f, radius, 0, 0));
	m_br.setModel(rotateTranslate(270.f, radius, width, 0));

	m_rectVertical.setModel(glm::translate(glm::mat4(1), glm::vec3(radius, 0, 0)));
	m_rectHorizontal.setModel(glm::translate(glm::mat4(1), glm::vec3(0, radius, 0)));
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
glm::mat4 RoundedRectangle<Vertex>::rotateTranslate(float deg, float radius, float tx, float ty)
{
	auto model = glm::translate(glm::mat4(1), glm::vec3(tx, ty, 0));
	model = glm::translate(model, glm::vec3(radius, radius, 0));
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0, 0, 1.f));
	return glm::translate(model, glm::vec3(-radius, -radius, 0));
}

template <typename Vertex>
void RoundedRectangle<Vertex>::doDraw(SceGxmContext *ctx, GeometryRenderer *renderer, const Camera *camera) const
{
	// prevent overlapping by stenciling
	sceGxmSetFrontStencilRef(ctx, 1);
	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_ALWAYS, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_REPLACE, 1, ~0);

	auto prevModel = m_bl.modelMatrix();
	m_bl.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_bl);
	m_bl.setModel(prevModel);

	prevModel = m_br.modelMatrix();
	m_br.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_br);
	m_br.setModel(prevModel);

	prevModel = m_tl.modelMatrix();
	m_tl.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_tl);
	m_tl.setModel(prevModel);

	prevModel = m_tr.modelMatrix();
	m_tr.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_tr);
	m_tr.setModel(prevModel);

	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_NOT_EQUAL, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_REPLACE, 1, ~0);

	prevModel = m_rectVertical.modelMatrix();
	m_rectVertical.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_rectVertical);
	m_rectVertical.setModel(prevModel);

	prevModel = m_rectHorizontal.modelMatrix();
	m_rectHorizontal.setModel(modelMatrix()*prevModel);
	renderer->draw(ctx, camera, &m_rectHorizontal);
	m_rectHorizontal.setModel(prevModel);

	sceGxmSetFrontStencilRef(ctx, 0);
	sceGxmSetFrontStencilFunc(ctx, SCE_GXM_STENCIL_FUNC_ALWAYS, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, SCE_GXM_STENCIL_OP_KEEP, 0, 0);
}

#endif // ROUNDEDRECTANGLE_H
