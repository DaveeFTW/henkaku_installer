/*
 * resource.cpp - utility for accessing internal and filesystem resources
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "resource.h"
#include <generatedresources.h>

#include <iostream>
#include <fstream>
#include <cstring>

using namespace Resource;

File Resource::read(const std::string& filename)
{
	std::string prefix("rsc:");
	
	// check if there is a rsc prefix
	if (filename.compare(0, prefix.length(), prefix) == 0)
	{
		return readResource(filename);
	}
	else if (filename.find(':') == std::string::npos)
	{
		return readFilesystem("app0:/" + filename);
	}

	return readFilesystem(filename);
}

File Resource::readResource(const std::string& filename)
{
	File file;
	auto res = ResourceFactory::find(filename);

	if (res)
	{
		file.data.resize(res->size);
		std::memcpy(file.data.data(), res->data, res->size);
		file.good = true;
	}

	return file;
}

File Resource::readFilesystem(const std::string& filename)
{
	File resource;
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();

	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	
	if (!file.good())
	{
		return resource;
	}
	
	resource.data = std::move(buffer);
	resource.good = true;
	return resource;
}
