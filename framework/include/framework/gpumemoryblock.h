/*
 * gpumemoryblock.h - utility class wrapping memory block allocation for use by the GPU
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GPUMEMORYBLOCK_H
#define GPUMEMORYBLOCK_H

#include <framework/memoryblock.h>

#include <psp2/gxm.h>

template <typename T>
class GpuMemoryBlock : public MemoryBlock<T>
{
public:
	GpuMemoryBlock(std::size_t size, SceGxmMemoryAttribFlags attributes, SceKernelMemBlockType type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE)
		: MemoryBlock<T>(size, type)
	{
		auto res = sceGxmMapMemory(MemoryBlock<T>::address(), MemoryBlock<T>::size(), attributes);

		if (res < 0)
		{
			// TODO: handle this error
		}
	}

	~GpuMemoryBlock(void)
	{
		sceGxmUnmapMemory(MemoryBlock<T>::address());
	}
};

class VertexUsseMemoryBlock : public MemoryBlock<char>
{
public:
	VertexUsseMemoryBlock(std::size_t size, SceKernelMemBlockType type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE)
		: MemoryBlock<char>(size, type)
	{
		auto res = sceGxmMapVertexUsseMemory(MemoryBlock<char>::address(), MemoryBlock<char>::size(), &m_offset);

		if (res < 0)
		{
			// TODO: handle this error
		}
	}

	~VertexUsseMemoryBlock(void)
	{
		sceGxmUnmapVertexUsseMemory(MemoryBlock<char>::address());
	}
	
	unsigned int offset(void) const
	{
		return m_offset;
	}

private:
	unsigned int m_offset;
};

class FragmentUsseMemoryBlock : public MemoryBlock<char>
{
public:
	FragmentUsseMemoryBlock(std::size_t size, SceKernelMemBlockType type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE)
		: MemoryBlock<char>(size, type)
	{
		auto res = sceGxmMapFragmentUsseMemory(MemoryBlock<char>::address(), MemoryBlock<char>::size(), &m_offset);

		if (res < 0)
		{
			// TODO: handle this error
		}
	}

	~FragmentUsseMemoryBlock(void)
	{
		sceGxmUnmapFragmentUsseMemory(MemoryBlock<char>::address());
	}
	
	unsigned int offset(void) const
	{
		return m_offset;
	}

private:
	unsigned int m_offset;
};

#endif // GPUMEMORYBLOCK_H
