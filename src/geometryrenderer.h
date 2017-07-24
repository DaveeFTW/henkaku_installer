/*
 * geometryrenderer.h - draw arbitrary geometry under single shader program
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GEOMETRYRENDERER_H
#define GEOMETRYRENDERER_H

#include <framework/gxmshaderprogram.h>
#include <framework/gxmvertexshader.h>
#include <framework/gxmfragmentshader.h>

class Geometry;
class Camera;

class GeometryRenderer
{
public:
	GeometryRenderer(GxmShaderPatcher *patcher);

	void setBlendInfo(SceGxmBlendInfo *blendInfo);

	template <typename Vertex>
	void setShaders(const std::string& vertexShader, const std::string& fragmentShader);
	void draw(SceGxmContext *ctx, const Camera *camera, const Geometry *geometry) const;

private:
	void readShaders(const std::string& vertexShader, const std::string& fragmentShader);

private:
	GxmShaderProgram m_program;
	mutable GxmVertexShader m_vertexShader;
	GxmFragmentShader m_fragmentShader;
	GxmShader::UniformIndex m_mvpIndex;
};

template <typename Vertex>
void GeometryRenderer::setShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
	// read shaders
	readShaders(vertexShader, fragmentShader);

	m_program.addShader(&m_vertexShader);
	m_program.addShader(&m_fragmentShader);

	m_mvpIndex = m_vertexShader.uniformIndex("mvp");

	auto attributes = Vertex::attributes(&m_vertexShader);
	auto streams = Vertex::streams();

	m_program.setVertexAttributeFormat(attributes.data(), attributes.size());
	m_program.setVertexStreamFormat(streams.data(), streams.size());
	m_program.link();
}


#endif // GEOMETRYRENDERER_H
