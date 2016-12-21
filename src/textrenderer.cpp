/*
 * textrenderer.h - text geometry and drawing functionality
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "textrenderer.h"
#include "shaderutility.h"
#include "font.h"

#include <string>

#include <utf8cpp/utf8.h>

TextRenderer::TextRenderer(GxmShaderPatcher *patcher)
	: m_program(patcher)
{
	// read shaders
	ShaderUtility::read("rsc:/text.vert.cg.gxp", &m_vertexShader);
	ShaderUtility::read("rsc:/text.frag.cg.gxp", &m_fragmentShader);

	// add programs
	m_program.addShader(&m_vertexShader);
	m_program.addShader(&m_fragmentShader);

	m_mvpIndex = m_vertexShader.uniformIndex("mvp");
	
	SceGxmVertexAttribute attributes[3];
	SceGxmVertexStream streams[3];

	attributes[0].streamIndex = 0;
	attributes[0].offset = 0;
	attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[0].componentCount = 3;
	attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(m_vertexShader.attributeIndex("position"));

	attributes[1].streamIndex = 0;
	attributes[1].offset = sizeof(float)*3;
	attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[1].componentCount = 2;
	attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(m_vertexShader.attributeIndex("texCoord"));

	attributes[2].streamIndex = 0;
	attributes[2].offset = 0;
	attributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attributes[2].componentCount = 3;
	attributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(m_vertexShader.attributeIndex("position"));

	streams[0].stride = sizeof(TextGeometry::TextVertex);
	streams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	m_program.setVertexAttributeFormat(attributes, 2);
	m_program.setVertexStreamFormat(streams, 1);

	SceGxmBlendInfo blendInfo;
	blendInfo.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
	blendInfo.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	blendInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendInfo.colorMask = SCE_GXM_COLOR_MASK_ALL;
	m_program.setBlendInfo(&blendInfo);
	m_program.link();
}

void TextRenderer::setFont(Font *font)
{
	m_font = font;
}

TextGeometry *TextRenderer::generateTextGeometry(const std::string& text)
{
	utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
	utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());
	TextGeometry *geometry = new TextGeometry;

	std::vector<TextGeometry::TextVertex> vertices(utf8::distance(text.begin(), text.end())*4);
	std::vector<std::uint16_t> indices(utf8::distance(text.begin(), text.end())*3*2);

	int x = 0;
	int y = 0;
	std::size_t indiceCount = 0;

	for (auto i = 0; it != end; ++it)
	{
		auto glyphInfo = m_font->glyphInfo(*it);

		vertices[i+0].x = x;
		vertices[i+0].y = y;
		vertices[i+0].z = 0;
		vertices[i+0].u = glyphInfo.quad.bl.x;
		vertices[i+0].v = glyphInfo.quad.bl.y;

		vertices[i+1].x = x+glyphInfo.quad.size.x;
		vertices[i+1].y = y;
		vertices[i+1].z = 0;
		vertices[i+1].u = glyphInfo.quad.br.x;
		vertices[i+1].v = glyphInfo.quad.br.y;

		vertices[i+2].x = x;
		vertices[i+2].y = y+glyphInfo.quad.size.y;
		vertices[i+2].z = 0;
		vertices[i+2].u = glyphInfo.quad.tl.x;
		vertices[i+2].v = glyphInfo.quad.tl.y;

		vertices[i+3].x = x+glyphInfo.quad.size.x;
		vertices[i+3].y = y+glyphInfo.quad.size.y;
		vertices[i+3].z = 0;
		vertices[i+3].u = glyphInfo.quad.tr.x;
		vertices[i+3].v = glyphInfo.quad.tr.y;

		indices[indiceCount+0] = i+0;
		indices[indiceCount+1] = i+1;
		indices[indiceCount+2] = i+2;
		indices[indiceCount+3] = i+1;
		indices[indiceCount+4] = i+3;
		indices[indiceCount+5] = i+2;

		i += 4;
		indiceCount += 6;

		x += glyphInfo.advance.x;
	}

	geometry->setVertices(std::move(vertices));
	geometry->setIndices(std::move(indices));
	return geometry;
}

void TextRenderer::setMvp(glm::mat4 mvp)
{
	m_mvp = mvp;
}

void TextRenderer::draw(SceGxmContext *ctx, TextGeometry *geometry)
{
	m_program.bind(ctx);
	m_font->atlas()->bind(ctx, 0);

	void *uniform = nullptr;
	sceGxmReserveVertexDefaultUniformBuffer(ctx, &uniform);
	m_vertexShader.setUniformBuffer(uniform);
	m_vertexShader.setUniformValue(m_mvpIndex, m_mvp);

	sceGxmSetVertexStream(ctx, 0, geometry->vertices()->address());
	sceGxmDraw(ctx, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, geometry->indices()->address(), geometry->indices()->count());
}
