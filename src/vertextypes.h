/*
 * vertextypes.h - defines various vertex types
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VERTEXTYPES_H
#define VERTEXTYPES_H

#include <framework/gxmvertexshader.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <psp2/gxm.h>

struct ColouredGeometryVertex
{
	glm::vec3 position;
	glm::vec4 colour;

	static std::vector<SceGxmVertexAttribute> attributes(GxmVertexShader *vertexShader)
	{
		std::vector<SceGxmVertexAttribute> attributes(2);
		attributes[0].streamIndex = 0;
		attributes[0].offset = offsetof(ColouredGeometryVertex, position);
		attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attributes[0].componentCount = 3;
		attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("position"));

		attributes[1].streamIndex = 0;
		attributes[1].offset = offsetof(ColouredGeometryVertex, colour);
		attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attributes[1].componentCount = 4;
		attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("colour"));
		return attributes;
	}

	static std::vector<SceGxmVertexStream> streams(void)
	{
		std::vector<SceGxmVertexStream> streams(1);
		streams[0].stride = sizeof(ColouredGeometryVertex);
		streams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
		return streams;
	}
	
	constexpr static int streamCount(void)
	{
		return 1;
	}
};

struct ColouredTextureVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec4 colour;

	static std::vector<SceGxmVertexAttribute> attributes(GxmVertexShader *vertexShader)
	{
		std::vector<SceGxmVertexAttribute> attributes(3);
		attributes[0].streamIndex = 0;
		attributes[0].offset = offsetof(ColouredTextureVertex, position);
		attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attributes[0].componentCount = 3;
		attributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("position"));

		attributes[1].streamIndex = 0;
		attributes[1].offset = offsetof(ColouredTextureVertex, texCoord);
		attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attributes[1].componentCount = 2;
		attributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("texCoord"));

		attributes[2].streamIndex = 0;
		attributes[2].offset = offsetof(ColouredTextureVertex, colour);
		attributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attributes[2].componentCount = 4;
		attributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(vertexShader->attributeIndex("colour"));
		return attributes;
	}

	static std::vector<SceGxmVertexStream> streams(void)
	{
		std::vector<SceGxmVertexStream> streams(1);
		streams[0].stride = sizeof(ColouredTextureVertex);
		streams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
		return streams;
	}
	
	constexpr static int streamCount(void)
	{
		return 1;
	}
};

#endif // VERTEXTYPES_H
