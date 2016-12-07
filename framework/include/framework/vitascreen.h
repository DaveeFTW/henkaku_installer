/*
 * vitascreen.h - 
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VITASCREEN_H
#define VITASCREEN_H

#include <framework/screen.h>

#include <vector>

template <typename T>
class GpuMemoryBlock;
class FragmentUsseMemoryBlock;

struct SceGxmContext;
struct SceGxmRenderTarget;
struct SceGxmDepthStencilSurface;

class VitaScreen : public Screen
{
public:
	VitaScreen(void);
	~VitaScreen(void);

	void draw(void) override;
	
private:
	void setRenderBuffer(int index);

private:
	static void onSwapQueue(const void *data);
	
private:
	static constexpr int DISPLAY_QUEUE_MAX_PENDING = 3;
	
	using RgbaPixel = union alignas(4)
	{
		std::uint32_t rgba;
		
		struct 
		{
			std::uint8_t r;
			std::uint8_t g;
			std::uint8_t b;
			std::uint8_t a;
		} components;
	};
	
	class Framebuffer;
	using RingBuffer = GpuMemoryBlock<char>;
	using DepthBuffer = GpuMemoryBlock<char>;
	
	std::unique_ptr<Framebuffer> m_fb[DISPLAY_QUEUE_MAX_PENDING];
	std::unique_ptr<RingBuffer> m_vdmRingBuffer, m_vertexRingBuffer, m_fragmentRingBuffer;
	std::unique_ptr<FragmentUsseMemoryBlock> m_fragmentUsseRingBuffer;
	std::unique_ptr<DepthBuffer> m_depthBuffer;
	
	std::vector<char> m_hostMemory;
	SceGxmContext *m_context{nullptr};
	SceGxmRenderTarget *m_renderTarget{nullptr};
	SceGxmDepthStencilSurface *m_depthStencilSurface{nullptr};

	int m_currentRenderBufferIndex{0};
	int m_nextRenderBufferIndex{0};
};

#endif // VITASCREEN_H
