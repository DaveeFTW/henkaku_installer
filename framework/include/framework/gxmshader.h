/*
 * gxmshader.h - facility for creating gxm shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GXMSHADER_H
#define GXMSHADER_H

#include <string>

#include <glm/mat4x4.hpp>

struct SceGxmProgramParameter;
struct SceGxmProgram;

class GxmShader
{
public:
	using UniformIndex = const SceGxmProgramParameter *;
	
	enum Type
	{
		Invalid,
		Vertex,
		Fragment
	};

public:
	virtual ~GxmShader(void);
	
	bool loadFromBuffer(const char *data, std::size_t size);
	
	bool valid(void) const;
	Type type(void) const;

	const SceGxmProgram *program(void) const;

	void setUniformBuffer(void *buffer);
	
	UniformIndex uniformIndex(const char *name);
	UniformIndex uniformIndex(const std::string& name);

	template <typename... T>
	void setUniformValue(UniformIndex index, T... args)
	{
		constexpr auto count = sizeof...(args);
		float values[count] = { args... };
		setUniform(index, 0, count, values);
	}
	
	void setUniformValue(UniformIndex index, glm::mat4 mat4);

protected:
	GxmShader(void) = default;
	virtual bool analyseShader(const char *shader);

private:
	void setUniform(UniformIndex index, unsigned int offset, unsigned int count, const float *data);

private:
	void *m_uniformBuffer{nullptr};
	SceGxmProgram *m_shaderProgram{nullptr};
	bool m_valid{false};
};

#endif // GXMSHADER_H


