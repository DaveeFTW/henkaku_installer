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

#include <glm/gtc/type_ptr.hpp>
#include <psp2/gxm.h>

namespace
{
	bool isUniform(const SceGxmProgramParameter *param)
	{
		return (param && sceGxmProgramParameterGetCategory(param) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM);
	}

	bool isVertexShader(const SceGxmProgram *program)
	{
		return (sceGxmProgramGetType(program) == SCE_GXM_VERTEX_PROGRAM);
	}

	bool isFragmentShader(const SceGxmProgram *program)
	{
		return (sceGxmProgramGetType(program) == SCE_GXM_FRAGMENT_PROGRAM);
	}
}

GxmShader::~GxmShader(void)
{
	if (m_shaderProgram)
		delete[] reinterpret_cast<unsigned char *>(m_shaderProgram);
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
	m_valid = true;
	return true;
}

bool GxmShader::valid(void) const
{
	return m_valid;
}

GxmShader::Type GxmShader::type(void) const
{
	if (!m_valid)
		return GxmShader::Invalid;

	if (isVertexShader(m_shaderProgram))
		return GxmShader::Vertex;
	else if (isFragmentShader(m_shaderProgram))
		return GxmShader::Fragment;

	return GxmShader::Invalid;
}

const SceGxmProgram *GxmShader::program(void) const
{
	return m_shaderProgram;
}

void GxmShader::setUniformBuffer(void *buffer)
{
	m_uniformBuffer = buffer;
}

GxmShader::UniformIndex GxmShader::uniformIndex(const char *name)
{
	auto parameter = sceGxmProgramFindParameterByName(m_shaderProgram, name);

	if (!isUniform(parameter))
		return nullptr;

	return parameter;
}

GxmShader::UniformIndex GxmShader::uniformIndex(const std::string& name)
{
	return uniformIndex(name.c_str());
}

void GxmShader::setUniformValue(UniformIndex index, glm::mat4 mat4)
{
	setUniform(index, 0, 16, glm::value_ptr(mat4));
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
