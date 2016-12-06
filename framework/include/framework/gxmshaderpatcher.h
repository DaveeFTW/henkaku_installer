/*
 * gxmshaderpatcher.h - create and manage a shader patcher object
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <memory>

template <typename T>
class GpuMemoryBlock;
class VertexUsseMemoryBlock;
class FragmentUsseMemoryBlock;

struct SceGxmShaderPatcher;

class GxmShaderPatcher
{
public:
	GxmShaderPatcher(size_t bufferSize = 64*1024, size_t vertexUsseSize = 64*1024, size_t fragmentUsseSize = 64*1024);
	~GxmShaderPatcher(void);

	SceGxmShaderPatcher *id(void) const;
	
private:
	using PatcherMemoryBlock = GpuMemoryBlock<char>;

	SceGxmShaderPatcher *m_shaderPatcher;
	std::unique_ptr<PatcherMemoryBlock> m_buffer;
	std::unique_ptr<VertexUsseMemoryBlock> m_vertexUsseBuffer;
	std::unique_ptr<FragmentUsseMemoryBlock> m_fragmentUsseBuffer;
};
