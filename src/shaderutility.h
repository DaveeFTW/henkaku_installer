/*
 * shaderutility.h - common boilerplate code for shaders
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef SHADERUTILITY_H
#define SHADERUTILITY_H

#include "resource.h"
#include <easyloggingpp/easylogging++.h>

#include <string>

namespace ShaderUtility
{
	template <typename T>
	void read(const std::string& file, T* shader)
	{
		auto fileData = Resource::read(file);
		
		if (!fileData.good)
		{
			LOG(FATAL) << "failed to read resource: \"" << file << "\"";
		}
		
		auto res = shader->loadFromBuffer(fileData.data.data(), fileData.data.size());
		
		if (!res || !shader->valid())
		{
			LOG(FATAL) << "failed to load shader program \"" << file << "\"";
		}
	}
}

#endif // SHADERUTILITY_H
