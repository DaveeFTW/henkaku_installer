/*
 * gxmshaderprogram.h - facility for using gxm shader programs
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GXMSHADERPROGRAM_H
#define GXMSHADERPROGRAM_H

struct SceGxmContext;
struct SceGxmRegisteredProgram;
struct SceGxmVertexProgram;
struct SceGxmFragmentProgram;
struct SceGxmVertexStream;
struct SceGxmVertexAttribute;

class GxmShader;
class GxmShaderPatcher;

class GxmShaderProgram
{
public:
	GxmShaderProgram(const GxmShaderPatcher *patcher);
	~GxmShaderProgram(void);

	bool addShader(GxmShader *shader);

	void setVertexStreamFormat(SceGxmVertexStream *stream, unsigned int count);
	void setVertexAttributeFormat(SceGxmVertexAttribute *attributes, unsigned int count);

	bool link(void);
	void bind(SceGxmContext *context);

	bool isLinked(void) const;

private:
	const GxmShaderPatcher *m_patcher{nullptr};
	SceGxmRegisteredProgram *m_vertexShaderId{nullptr}, *m_fragmentShaderId{nullptr};
	SceGxmVertexStream *m_vertexStreams{nullptr};
	unsigned int m_vertexStreamCount{0};
	SceGxmVertexAttribute *m_vertexAttributes{nullptr};
	unsigned int m_vertexAttributeCount{0};
	SceGxmVertexProgram *m_vertexProgram{nullptr};
	SceGxmFragmentProgram *m_fragmentProgram{nullptr};
	bool m_linked{false};
};

#endif // GXMSHADERPROGRAM_H
