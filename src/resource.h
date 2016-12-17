/*
 * resource.h - utility for accessing internal and filesystem resources
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include <vector>
#include <string>

namespace Resource
{
	struct File
	{
		std::vector<char> data;
		bool good{false};
	};

	File read(const std::string& filename);
	File readResource(const std::string& filename);
	File readFilesystem(const std::string& filename);
}

#endif // RESOURCE_H
