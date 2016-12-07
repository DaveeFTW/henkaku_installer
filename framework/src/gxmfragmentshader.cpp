/*
 * gxmfragmentshader.h - facility for using gxm fragment shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmfragmentshader.h>
#include <psp2/gxm.h>

bool GxmFragmentShader::analyseShader(const unsigned char *shader)
{
	if (!GxmShader::analyseShader(shader))
		return false;

	auto program = reinterpret_cast<const SceGxmProgram *>(shader);

	// check its a fragment shader
	return (sceGxmProgramGetType(program) == SCE_GXM_FRAGMENT_PROGRAM);
}
