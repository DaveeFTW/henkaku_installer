/*
 * gxmshaderprogram.h - facility for using gxm shader programs
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmshaderprogram.h>
#include <framework/gxmshaderpatcher.h>
#include <framework/gxmshader.h>

#include <easyloggingpp/easylogging++.h>

#include <psp2/gxm.h>

GxmShaderProgram::GxmShaderProgram(const GxmShaderPatcher *patcher)
	: m_patcher(patcher)
{
}

GxmShaderProgram::~GxmShaderProgram(void)
{
	if (m_fragmentProgram)
	{
		sceGxmShaderPatcherReleaseFragmentProgram(m_patcher->id(), m_fragmentProgram);
		m_fragmentProgram = nullptr;
	}

	if (m_vertexProgram)
	{
		sceGxmShaderPatcherReleaseVertexProgram(m_patcher->id(), m_vertexProgram);
		m_vertexProgram = nullptr;
	}

	if (m_vertexShaderId)
	{
		sceGxmShaderPatcherUnregisterProgram(m_patcher->id(), m_vertexShaderId);
		m_vertexShaderId = nullptr;
	}

	if (m_fragmentShaderId)
	{
		sceGxmShaderPatcherUnregisterProgram(m_patcher->id(), m_fragmentShaderId);
		m_fragmentShaderId = nullptr;
	}
}

bool GxmShaderProgram::addShader(GxmShader *shader)
{
	if (!shader->valid())
		return false;

	SceGxmRegisteredProgram *program = nullptr;
	auto res = sceGxmShaderPatcherRegisterProgram(m_patcher->id(), shader->program(), &program);
	
	if (res < 0)
	{
		return false;
	}

	if (shader->type() == GxmShader::Vertex)
	{
		if (m_vertexProgram)
		{
			sceGxmShaderPatcherReleaseVertexProgram(m_patcher->id(), m_vertexProgram);
			m_vertexProgram = nullptr;
		}

		if (m_vertexShaderId)
		{
			sceGxmShaderPatcherUnregisterProgram(m_patcher->id(), m_vertexShaderId);
		}

		m_vertexShaderId = program;
	}
	else if (shader->type() == GxmShader::Fragment)
	{
		if (m_fragmentProgram)
		{
			sceGxmShaderPatcherReleaseFragmentProgram(m_patcher->id(), m_fragmentProgram);
			m_fragmentProgram = nullptr;
		}

		if (m_fragmentShaderId)
		{
			sceGxmShaderPatcherUnregisterProgram(m_patcher->id(), m_fragmentShaderId);
		}

		m_fragmentShaderId = program;
	}

	m_linked = false;
	return true;
}

void GxmShaderProgram::setBlendInfo(SceGxmBlendInfo *info)
{
	m_blendInfo = info;
}

void GxmShaderProgram::setVertexStreamFormat(SceGxmVertexStream *stream, unsigned int count)
{
	m_vertexStreams = stream;
	m_vertexStreamCount = count;
}

void GxmShaderProgram::setVertexAttributeFormat(SceGxmVertexAttribute *attributes, unsigned int count)
{
	m_vertexAttributes = attributes;
	m_vertexAttributeCount = count;
}

bool GxmShaderProgram::isLinked(void) const
{
	return m_linked;
}

bool GxmShaderProgram::link(void)
{
	if (!m_vertexShaderId || !m_fragmentShaderId)
		return false;

	if (m_linked)
		return false;

	auto res = sceGxmShaderPatcherCreateVertexProgram(
		  m_patcher->id()
		, m_vertexShaderId
		, m_vertexAttributes
		, m_vertexAttributeCount
		, m_vertexStreams
		, m_vertexStreamCount
		, &m_vertexProgram
	);
	LOG(INFO) << "sceGxmShaderPatcherCreateVertexProgram res: " << res;

	res = sceGxmShaderPatcherCreateFragmentProgram(
		  m_patcher->id()
		, m_fragmentShaderId
		, SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4
		, SCE_GXM_MULTISAMPLE_NONE
		, m_blendInfo
		, sceGxmShaderPatcherGetProgramFromId(m_vertexShaderId)
		, &m_fragmentProgram
	);
	LOG(INFO) << "sceGxmShaderPatcherCreateFragmentProgram res: " << res;

	m_linked = true;
	return true;
}

void GxmShaderProgram::bind(SceGxmContext *context) const
{
	sceGxmSetVertexProgram(context, m_vertexProgram);
	sceGxmSetFragmentProgram(context, m_fragmentProgram);
}
