/*
 * memoryblock.h - utility functions wrapping memory block allocation
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include <framework/bitwise.h>

#include <cstring>

#include <psp2/kernel/sysmem.h>

#include <easyloggingpp/easylogging++.h>

extern int g_counter;

template <typename T>
class MemoryBlock
{
	// TODO: contribute to SDK instead
	static constexpr int SCE_KERNEL_MEMBLOCK_ENABLE_ALIGNMENT = 0x00000004;
	
public:
	MemoryBlock(std::size_t elements, SceKernelMemBlockType type = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW)
	{
		SceKernelAllocMemBlockOpt *popt = nullptr, opt;

		if (requireAlignment(alignof(T), type))
		{
			std::memset(&opt, 0, sizeof(opt));
		
			opt.size = sizeof(opt);
			opt.attr = SCE_KERNEL_MEMBLOCK_ENABLE_ALIGNMENT;
			opt.alignment = alignNearestPow2(alignof(T));
			
			popt = &opt;
		}
		
		auto size = elements * sizeof(T);
		m_count = elements;
		
		// align our allocation size appropriately
		m_size = alignMultiple(size, type);
		
		// allocate the memory block from the system
		m_uid = sceKernelAllocMemBlock("", type, m_size, popt);
		
		if (m_uid < 0)
		{
			LOG(FATAL) << "MemoryBlock allocate failure: " << m_uid << ". sceKernelAllocMemBlock(\"\", " << type << ", " << m_size << ", " << (unsigned int)popt << ")";
			return;
		}
		
		auto res = sceKernelGetMemBlockBase(m_uid, reinterpret_cast<void **>(&m_address));
		
		if (res < 0)
		{
			LOG(FATAL) << "MemoryBlock get block failure: " << m_uid << ". sceKernelGetMemBlockBase(" << m_uid << ", " << (unsigned int)reinterpret_cast<void **>(&m_address) << ")";
			return;
		}
	}
	
	virtual ~MemoryBlock(void)
	{
		sceKernelFreeMemBlock(m_uid);
		m_uid = -1;
		m_address = nullptr;
		m_size = 0;
	}
	
	T *address(void) const
	{
		return m_address;
	}
	
	std::size_t size(void) const
	{
		return m_size;
	}
	
	std::size_t count(void) const
	{
		return m_count;
	}
	
private:
	bool requireAlignment(std::size_t alignment, SceKernelMemBlockType type)
	{
		switch (type)
		{
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW:
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE:
			if (alignment < 4*1024)
				return false;
			
			if (alignment > 16*1024*1024)
			{
				// TODO: handle this error
				return false;
			}

			return true;
			
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW:
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW:
			return false;
		
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW:
			if (alignment < 256*1024)
				return false;
			
			if (alignment > 16*1024*1024)
			{
				// TODO: handle this error
				return false;
			}

			return true;
		}
		
		return false;
	}
	
	std::size_t alignMultiple(std::size_t size, SceKernelMemBlockType type)
	{
		
		switch (type)
		{
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW:
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE:
			// must be aligned by 4KB
			size = alignPow2(size, 4*1024);
			break;
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW:
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW:
			// must be aligned by 1MB
			size = alignPow2(size, 1*1024*1024);
			break;
		case SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW:
			// must be aligned by 256KB
			size = alignPow2(size, 256*1024);
			break;
		}
		
		return size;
	}
	
private:
	SceUID m_uid{-1};
	T *m_address{nullptr};
	std::size_t m_size{0}, m_count{0};
};

#endif // MEMORYBLOCK_H
