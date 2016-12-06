/*
 * gxmshader.cpp - facility for creating gxm shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmshader.h>

#include <cstring>

#include <psp2/gxm.h>

namespace
{
	bool isUniform(const SceGxmProgramParameter *param)
	{
		return (param && sceGxmProgramParameterGetCategory(param) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM);
	}
}

GxmShader::~GxmShader(void)
{
	if (m_shaderProgram)
		delete[] reinterpret_cast<char *>(m_shaderProgram);
}

bool GxmShader::loadFromBuffer(const char *shader, std::size_t size)
{
	auto program = new char[size];
	std::memcpy(program, shader, size);
	
	if (!analyseShader(program))
	{
		delete[] program;
		return false;
	}
	
	if (m_shaderProgram)
		delete[] reinterpret_cast<char *>(m_shaderProgram);

	m_shaderProgram = reinterpret_cast<SceGxmProgram *>(program);
	return true;
}

void GxmShader::setUniformBuffer(void *buffer)
{
	m_uniformBuffer = buffer;
}

GxmShader::UniformIndex GxmShader::uniformIndex(const char *name)
{
	auto parameter = sceGxmProgramFindParameterByName(nullptr, name);

	if (!isUniform(parameter))
		return nullptr;

	return parameter;
}

GxmShader::UniformIndex GxmShader::uniformIndex(const std::string& name)
{
	return uniformIndex(name.c_str());
}

void GxmShader::setUniform(GxmShader::UniformIndex index, unsigned int offset, unsigned int count, const float *data)
{
	sceGxmSetUniformDataF(m_uniformBuffer, index, offset, count, data);
}

bool GxmShader::analyseShader(const char *shader)
{
	auto program = reinterpret_cast<const SceGxmProgram *>(shader);
	
	if (sceGxmProgramCheck(program) < 0)
	{
		return false;
	}

	return true;
}
