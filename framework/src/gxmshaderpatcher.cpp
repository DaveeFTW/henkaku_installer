/*
 * gxmshaderpatcher.cpp - create and manage a shader patcher object
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
 
#include <framework/gxmshaderpatcher.h>
#include <framework/gpumemoryblock.h>

#include <cstring>

#include <psp2/gxm.h>

namespace 
{
	void *hostAlloc(void *user, unsigned int size)
	{
		return new char[size];
	}

	void hostFree(void *user, void *ptr)
	{
		delete[] reinterpret_cast<char *>(ptr);
	}
} // anonymous namespace

GxmShaderPatcher::GxmShaderPatcher(size_t bufferSize, size_t vertexUsseSize, size_t fragmentUsseSize)
	: m_buffer(std::make_unique<PatcherMemoryBlock>
	(
		bufferSize,
		SCE_GXM_MEMORY_ATTRIB_RW
	))
	, m_vertexUsseBuffer(std::make_unique<VertexUsseMemoryBlock>
	(
		vertexUsseSize
	))
	, m_fragmentUsseBuffer(std::make_unique<FragmentUsseMemoryBlock>
	(
		fragmentUsseSize
	))
{
	// create our parameter structure for shader patcher
	SceGxmShaderPatcherParams params;
	std::memset(&params, 0, sizeof(params));

	params.userData = this;
	params.hostAllocCallback = &hostAlloc;
	params.hostFreeCallback = &hostFree;
	params.bufferAllocCallback = nullptr;
	params.bufferFreeCallback = nullptr;
	params.bufferMem = m_buffer->address();
	params.bufferMemSize = m_buffer->size();
	params.vertexUsseAllocCallback = nullptr;
	params.vertexUsseFreeCallback = nullptr;
	params.vertexUsseMem = m_vertexUsseBuffer->address();
	params.vertexUsseMemSize = m_vertexUsseBuffer->size();
	params.vertexUsseOffset = m_vertexUsseBuffer->offset();
	params.fragmentUsseAllocCallback = nullptr;
	params.fragmentUsseFreeCallback = nullptr;
	params.fragmentUsseMem = m_fragmentUsseBuffer->address();
	params.fragmentUsseMemSize = m_fragmentUsseBuffer->size();
	params.fragmentUsseOffset = m_fragmentUsseBuffer->offset();

	sceGxmShaderPatcherCreate(&params, &m_shaderPatcher);
}

GxmShaderPatcher::~GxmShaderPatcher(void)
{
	sceGxmShaderPatcherDestroy(m_shaderPatcher);
}

SceGxmShaderPatcher *GxmShaderPatcher::id(void) const
{
	return m_shaderPatcher;
}
