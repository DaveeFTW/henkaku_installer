/*
 * gxmvertexshader.h - facility for using gxm vertex shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmvertexshader.h>
#include <psp2/gxm.h>

namespace
{
	bool isAttribute(const SceGxmProgramParameter *param)
	{
		return (param && sceGxmProgramParameterGetCategory(param) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE);
	}
} // anonymous namespace

GxmVertexShader::AttributeIndex GxmVertexShader::attributeIndex(const char *name)
{
	auto parameter = sceGxmProgramFindParameterByName(program(), name);

	if (!isAttribute(parameter))
		return nullptr;

	return parameter;
}

GxmVertexShader::AttributeIndex GxmVertexShader::attributeIndex(const std::string& name)
{
	return attributeIndex(name.c_str());
}

bool GxmVertexShader::analyseShader(const char *shader)
{
	if (!GxmShader::analyseShader(shader))
		return false;

	auto program = reinterpret_cast<const SceGxmProgram *>(shader);

	// check its a vertex shader
	return (sceGxmProgramGetType(program) == SCE_GXM_VERTEX_PROGRAM);
}
