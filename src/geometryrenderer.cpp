/*
 * geometryrenderer.cpp - draw arbitrary geometry under single shader program
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "geometryrenderer.h"
#include "shaderutility.h"
#include "camera.h"
#include "geometry.h"

#include <psp2/gxm.h>

GeometryRenderer::GeometryRenderer(GxmShaderPatcher *patcher)
	: m_program(patcher)
{
	
}

void GeometryRenderer::setBlendInfo(SceGxmBlendInfo *blendInfo)
{
	m_program.setBlendInfo(blendInfo);
}

void GeometryRenderer::readShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
	ShaderUtility::read(vertexShader, &m_vertexShader);
	ShaderUtility::read(fragmentShader, &m_fragmentShader);
}

void GeometryRenderer::draw(SceGxmContext *ctx, const Camera *camera, const Geometry *geometry)
{
	m_program.bind(ctx);

	void *uniform = nullptr;
	sceGxmReserveVertexDefaultUniformBuffer(ctx, &uniform);
	m_vertexShader.setUniformBuffer(uniform);
	m_vertexShader.setUniformValue(m_mvpIndex, camera->projectionMatrix() * camera->viewMatrix() * geometry->modelMatrix());

	geometry->draw(ctx, this, camera);
}