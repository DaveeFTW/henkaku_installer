/*
 * gxmfragmentshader.h - facility for using gxm fragment shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GXMFRAGMENTSHADER_H
#define GXMFRAGMENTSHADER_H

#include <framework/gxmshader.h>

class GxmFragmentShader : public GxmShader
{
public:
	GxmFragmentShader(void) = default;
	virtual ~GxmFragmentShader(void) = default;

protected:
	bool analyseShader(const char *shader) override;
};

#endif // GXMFRAGMENTSHADER_H
