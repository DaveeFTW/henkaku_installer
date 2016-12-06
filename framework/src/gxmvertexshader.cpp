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

bool GxmVertexShader::analyseShader(const char *shader)
{
	if (!GxmShader::analyseShader(shader))
		return false;

	auto program = reinterpret_cast<const SceGxmProgram *>(shader);

	// check its a vertex shader
	return (sceGxmProgramGetType(program) == SCE_GXM_VERTEX_PROGRAM);
}
