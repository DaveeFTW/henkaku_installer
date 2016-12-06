/*
 * gxmshader.cpp - facility for creating gxm shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmshader.h>

#include <psp2/gxm.h>

namespace
{
	bool isUniform(const SceGxmProgramParameter *param)
	{
		return (param && sceGxmProgramParameterGetCategory(param) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM);
	}
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
