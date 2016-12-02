/*
 * screen.cpp - abstract class for managing rendering screen
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/vitascreen.h>
#include <framework/guiapplication.h>
#include <framework/view.h>
#include <framework/gpumemoryblock.h>

#include <psp2/gxm.h>
#include <psp2/display.h>

int g_counter = 0;

namespace 
{
	static constexpr std::size_t DISPLAY_WIDTH = 960;
	static constexpr std::size_t DISPLAY_HEIGHT = 544;
	static constexpr std::size_t DISPLAY_STRIDE = 1024;

	using CallbackData = struct
	{
		VitaScreen *obj;
		int fbIndex;
	};
} // anonymous namespace

class VitaScreen::Framebuffer
{
public:
	Framebuffer(void)
		: m_buffer(std::make_unique<FramebufferMemory>(
			DISPLAY_HEIGHT*DISPLAY_STRIDE, 
			SCE_GXM_MEMORY_ATTRIB_RW, 
			SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW)
		)
	{
		g_counter = m_buffer->size();
		std::memset(&m_framebuf, 0, sizeof(m_framebuf));
		
		m_framebuf.size = sizeof(m_framebuf);
		m_framebuf.base = m_buffer->address();
		m_framebuf.pitch = DISPLAY_STRIDE;
		m_framebuf.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
		m_framebuf.width = DISPLAY_WIDTH;
		m_framebuf.height = DISPLAY_HEIGHT;
		
		sceGxmColorSurfaceInit(&m_colorSurface,
			SCE_GXM_COLOR_FORMAT_A8B8G8R8,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			SCE_GXM_COLOR_SURFACE_SCALE_NONE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			DISPLAY_WIDTH,
			DISPLAY_HEIGHT,
			DISPLAY_STRIDE,
			m_buffer->address());
		
		sceGxmSyncObjectCreate(&m_syncObject);
	}

	~Framebuffer(void)
	{
		sceGxmSyncObjectDestroy(m_syncObject);
	}
	
	RgbaPixel *address(void) const
	{
		return m_buffer->address();
	}
	
	SceGxmSyncObject *sync(void)
	{
		return m_syncObject;
	}

	SceGxmColorSurface *surface(void)
	{
		return &m_colorSurface;
	}

	void setAsFrameBuffer(void)
	{
		sceDisplaySetFrameBuf(&m_framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
	}
	
private:
	using FramebufferMemory = GpuMemoryBlock<RgbaPixel>;
	std::unique_ptr<FramebufferMemory> m_buffer;
	SceGxmSyncObject *m_syncObject;
	SceGxmColorSurface m_colorSurface;
	SceDisplayFrameBuf m_framebuf;
};

void VitaScreen::onSwapQueue(const void *data)
{
	auto cb = static_cast<const CallbackData *>(data);
	cb->obj->setRenderBuffer(cb->fbIndex);
}

VitaScreen::VitaScreen(void)
	: m_vdmRingBuffer(std::make_unique<RingBuffer>(
		SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE, 
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_vertexRingBuffer(std::make_unique<RingBuffer>(
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE, 
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_fragmentRingBuffer(std::make_unique<RingBuffer>(
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE, 
		SCE_GXM_MEMORY_ATTRIB_READ
	))
	, m_fragmentUsseRingBuffer(std::make_unique<FragmentUsseMemoryBlock>(
		SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE
	))
	, m_hostMemory(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE)
	, m_depthStencilSurface(new SceGxmDepthStencilSurface)
{
	SceGxmInitializeParams initParams;
	
	initParams.flags = 0; // CPU 0?
	initParams.displayQueueMaxPendingCount = DISPLAY_QUEUE_MAX_PENDING;
	initParams.displayQueueCallback = &VitaScreen::onSwapQueue;
	initParams.displayQueueCallbackDataSize = sizeof(CallbackData);
	initParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

	// initialize gxm
	sceGxmInitialize(&initParams);
	
	SceGxmContextParams contextParams;
	
	// create our context params
	contextParams.hostMem = m_hostMemory.data();
	contextParams.hostMemSize = m_hostMemory.size();
	contextParams.vdmRingBufferMem = m_vdmRingBuffer->address();
	contextParams.vdmRingBufferMemSize = m_vdmRingBuffer->size();
	contextParams.vertexRingBufferMem = m_vertexRingBuffer->address();
	contextParams.vertexRingBufferMemSize = m_vertexRingBuffer->size();
	contextParams.fragmentRingBufferMem = m_fragmentRingBuffer->address();
	contextParams.fragmentRingBufferMemSize = m_fragmentRingBuffer->size();
	contextParams.fragmentUsseRingBufferMem = m_fragmentUsseRingBuffer->address();
	contextParams.fragmentUsseRingBufferMemSize = m_fragmentUsseRingBuffer->size();
	contextParams.fragmentUsseRingBufferOffset = m_fragmentUsseRingBuffer->offset();
	
	// create our main context
	sceGxmCreateContext(&contextParams, &m_context);
	
	// next we create a render target, in this case for screen
	SceGxmRenderTargetParams renderTargetParams;
	renderTargetParams.flags = 0;
	renderTargetParams.width = DISPLAY_WIDTH;
	renderTargetParams.height = DISPLAY_HEIGHT;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = SCE_GXM_MULTISAMPLE_NONE;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = -1;
	
	sceGxmCreateRenderTarget(&renderTargetParams, &m_renderTarget);
	
	// allocate our framebuffers
	for (int i = 0; i < DISPLAY_QUEUE_MAX_PENDING; ++i)
	{
		m_fb[i] = std::make_unique<Framebuffer>();
	}

	// create depth buffer
	m_depthBuffer = std::make_unique<DepthBuffer>(
		alignPow2(DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX)*alignPow2(DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY),
		SCE_GXM_MEMORY_ATTRIB_RW
	);
	
	sceGxmDepthStencilSurfaceInit(m_depthStencilSurface,
		SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		alignPow2(DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX),
		m_depthBuffer->address(),
		nullptr);
}

VitaScreen::~VitaScreen(void)
{
	delete m_depthStencilSurface;
	
	if (m_renderTarget)
		sceGxmDestroyRenderTarget(m_renderTarget);
	
	m_renderTarget = nullptr;
	
	if (m_context)
		sceGxmDestroyContext(m_context);
	
	m_context = nullptr;
}

void VitaScreen::draw(void)
{
	auto views = GuiApplication::allViews();
	
	sceGxmBeginScene(m_context, 
		0, 
		m_renderTarget, 
		nullptr, 
		nullptr, 
		m_fb[m_nextRenderBufferIndex]->sync(), 
		m_fb[m_nextRenderBufferIndex]->surface(), 
		m_depthStencilSurface);
	
	for (auto view : views)
	{
		view->render(m_fb[m_nextRenderBufferIndex]->address());
	}
	
	sceGxmEndScene(m_context, nullptr, nullptr);
	sceGxmPadHeartbeat(m_fb[m_nextRenderBufferIndex]->surface(), m_fb[m_nextRenderBufferIndex]->sync());
	
	CallbackData data;
	data.obj = this;
	data.fbIndex = m_nextRenderBufferIndex;

	sceGxmDisplayQueueAddEntry(
		m_fb[m_currentRenderBufferIndex]->sync(),
		m_fb[m_nextRenderBufferIndex]->sync(),
		&data
	);

	m_currentRenderBufferIndex = m_nextRenderBufferIndex;
	m_nextRenderBufferIndex = (m_nextRenderBufferIndex + 1) % DISPLAY_QUEUE_MAX_PENDING;
}

void VitaScreen::setRenderBuffer(int index)
{
	m_fb[index]->setAsFrameBuffer();
}
