/*
 * gxmvertexshader.h - facility for using gxm vertex shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GXMVERTEXSHADER_H
#define GXMVERTEXSHADER_H

#include <framework/gxmshader.h>

class GxmVertexShader : public GxmShader
{
public:
	using AttributeIndex = const SceGxmProgramParameter *;

public:
	GxmVertexShader(void) = default;
	virtual ~GxmVertexShader(void) = default;

	AttributeIndex attributeIndex(const char *name);
	AttributeIndex attributeIndex(const std::string& name);

protected:
	bool analyseShader(const unsigned char *shader) override;
};

#endif // GXMVERTEXSHADER_H
