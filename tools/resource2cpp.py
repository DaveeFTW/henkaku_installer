#!/usr/bin/python
import sys, os, re, json, errno

def clean(s):
	# Remove invalid characters
	s = re.sub('[^0-9a-zA-Z_]', '', s)
	# Remove leading characters until we find a letter or underscore
	s = re.sub('^[^a-zA-Z_]+', '', s)
	return s

def generate_cpp(filename, header, cdata):
	initialisations = ""
	buffers = ""
	
	for key in cdata:
		initialisations += "\t\t{ \"%s\", { %s, sizeof(%s) } },\n" % (key, clean(key[5:]), clean(key[5:]))
		buffers += "\t%s\n" % (cdata[key])

	initialisations = initialisations[:-2]
	buffers = buffers[:-1]

	return """/*
 * %s - AUTOGENERATED RESOURCE FILE
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include \"%s\"

namespace {
%s
}

ResourceFactory::ResourceFactory(std::unordered_map<std::string, Resource> resources)
	: m_resources(resources)
{
}

ResourceFactory *ResourceFactory::instance(void)
{
	static ResourceFactory factory(
		// initialisations go here
	{
%s
	});

	return &factory;
}
""" % (filename, os.path.basename(header), buffers, initialisations)

def generate_hpp(filename):
	return """/*
 * %s - AUTOGENERATED RESOURCE FILE
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef AUTOGEN_%s_RESOURCE_H
#define AUTOGEN_%s_RESOURCE_H

#include <unordered_map>
#include <string>

class ResourceFactory
{
public:
	struct Resource
	{
		const char *data;
		unsigned int size;
	};

public:
	static const Resource *find(const std::string& name)
	{
		auto it = instance()->m_resources.find(name);

		if (it == instance()->m_resources.end())
			return nullptr;
		
		return &(it->second);
	}

private:
	ResourceFactory(std::unordered_map<std::string, Resource> resources);
	static ResourceFactory *instance(void);

private:
	std::unordered_map<std::string, Resource> m_resources;
};

#endif // AUTOGEN_%s_RESOURCE_H
""" % (filename, clean(filename), clean(filename), clean(filename))

if __name__ == "__main__":
	cdata = dict()
	for res in sys.argv[3:]:
		with open(res, 'r') as f:
			data = f.read()
			cdata.update(json.loads(data))
	
	resource_cpp = generate_cpp(sys.argv[1], sys.argv[2], cdata)
	resource_hpp = generate_hpp(sys.argv[2])

	if os.path.dirname(sys.argv[1]) != "":
		if not os.path.exists(os.path.dirname(sys.argv[1])):
			try:
				os.makedirs(os.path.dirname(sys.argv[1]))
			except OSError as exc: # Guard against race condition
				if exc.errno != errno.EEXIST:
					raise

	with open(sys.argv[1], "w") as f:
		f.write(resource_cpp)
	
	if os.path.dirname(sys.argv[2]) != "":
		if not os.path.exists(os.path.dirname(sys.argv[2])):
			try:
				os.makedirs(os.path.dirname(sys.argv[2]))
			except OSError as exc: # Guard against race condition
				if exc.errno != errno.EEXIST:
					raise

	with open(sys.argv[2], "w") as f:
		f.write(resource_hpp)