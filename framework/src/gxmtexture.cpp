/*
 * gxmtexture.cpp - encapsulate a single gxm texture
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/gxmtexture.h>
#include <framework/gpumemoryblock.h>

#include <easyloggingpp/easylogging++.h>

#include <psp2/gxm.h>

namespace
{
	constexpr SceGxmTextureAddrMode convertWrapMode(GxmTexture::WrapMode mode)
	{
		switch (mode)
		{
		default:
		case GxmTexture::Repeat:
			return SCE_GXM_TEXTURE_ADDR_REPEAT;
		case GxmTexture::MirroredRepeat:
			return SCE_GXM_TEXTURE_ADDR_MIRROR;
		case GxmTexture::ClampToEdge:
			return SCE_GXM_TEXTURE_ADDR_CLAMP;
		case GxmTexture::MirrorClamp:
			return SCE_GXM_TEXTURE_ADDR_MIRROR_CLAMP;
		case GxmTexture::RepeatIgnoreBorder:
			return SCE_GXM_TEXTURE_ADDR_REPEAT_IGNORE_BORDER;
		case GxmTexture::ClampFullBorder:
			return SCE_GXM_TEXTURE_ADDR_CLAMP_FULL_BORDER;
		case GxmTexture::ClampIgnoreBorder:
			return SCE_GXM_TEXTURE_ADDR_CLAMP_IGNORE_BORDER;
		case GxmTexture::ClampHalfBorder:
			return SCE_GXM_TEXTURE_ADDR_CLAMP_HALF_BORDER;
		}
	}

	constexpr SceGxmTextureMipFilter convertFilter(GxmTexture::MipFilter filter)
	{
		switch (filter)
		{
		default:
		case GxmTexture::MipFilter::Disabled:
			return SCE_GXM_TEXTURE_MIP_FILTER_DISABLED;
		case GxmTexture::MipFilter::Enabled:
			return SCE_GXM_TEXTURE_MIP_FILTER_ENABLED;
		}
	}

	constexpr SceGxmTextureFilter convertFilter(GxmTexture::Filter filter)
	{
		switch (filter)
		{
		default:
		case GxmTexture::Point:
			return SCE_GXM_TEXTURE_FILTER_POINT;
		case GxmTexture::Linear:
			return SCE_GXM_TEXTURE_FILTER_LINEAR;
		}
	}

	constexpr SceGxmTextureFormat convertTextureFormat(GxmTexture::TextureFormat format)
	{
		switch (format)
		{
		default:
		case GxmTexture::ARGB8:
			return SCE_GXM_TEXTURE_FORMAT_A8R8G8B8;
		case GxmTexture::ABGR8:
			return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
		case GxmTexture::U8_R111:
			return SCE_GXM_TEXTURE_FORMAT_U8_R111;
		}
	}

	constexpr std::size_t texturePixelSize(GxmTexture::TextureFormat format)
	{
		switch (format)
		{
		default:
		case GxmTexture::ARGB8:
		case GxmTexture::ABGR8:
			return 4;
		case GxmTexture::U8_R111:
			return 1;
		}
	}
} // anonymous namespace

GxmTexture::GxmTexture(void)
	: m_texture(new SceGxmTexture)
{
	std::memset(m_texture.get(), 0, sizeof(SceGxmTexture));
}

void GxmTexture::allocateStorage(void)
{
	if (m_storage)
		return;

	m_storage = std::make_unique<GpuMemoryBlock<char>>
	(
		  m_width*m_height*m_depth*texturePixelSize(m_format)
		, SCE_GXM_MEMORY_ATTRIB_READ
		, SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW
	);
}

bool GxmTexture::isStorageAllocated(void) const
{
	return m_storage != nullptr;
}

void GxmTexture::bind(SceGxmContext *ctx, std::uint32_t unit) const
{
	sceGxmSetFragmentTexture(ctx, unit, m_texture.get());
}

void GxmTexture::setSize(std::size_t width, std::size_t height, std::size_t depth)
{
	m_width = width;
	m_height = height;
	m_depth = depth;
}

void GxmTexture::setData(const void *data)
{
	if (data == nullptr)
	{
		std::memset(m_storage->address(), 0, m_width*m_height*m_depth*texturePixelSize(m_format));
	}
	else
	{
		std::memcpy(m_storage->address(), data, m_width*m_height*m_depth*texturePixelSize(m_format));
	}

	auto res = sceGxmTextureInitLinear
	(
		m_texture.get(),
		m_storage->address(),
		convertTextureFormat(m_format),
		m_width,
		m_height,
		0
	);

	LOG(INFO) << "sceGxmTextureInitLinear: " << res;
}

void GxmTexture::setEmptyData(void)
{
	setData(nullptr);
}

void GxmTexture::setMinificationFilter(Filter filter)
{
	sceGxmTextureSetMinFilter(m_texture.get(), convertFilter(filter));
}

void GxmTexture::setMagnificationFilter(Filter filter)
{
	sceGxmTextureSetMagFilter(m_texture.get(), convertFilter(filter));
}

void GxmTexture::setMinMagFilter(Filter min, Filter mag)
{
	setMinificationFilter(min);
	setMagnificationFilter(mag);
}

void GxmTexture::setWrapMode(CoordinateDirection direction, WrapMode mode)
{
	if (direction == DirectionU)
	{
		sceGxmTextureSetUAddrMode(m_texture.get(), convertWrapMode(mode));
	}
	else if (direction == DirectionV)
	{
		sceGxmTextureSetVAddrMode(m_texture.get(), convertWrapMode(mode));
	}
}

void GxmTexture::setWrapMode(WrapMode mode)
{
	setWrapMode(DirectionU, mode);
	setWrapMode(DirectionV, mode);
}

void GxmTexture::setFormat(TextureFormat format)
{
	m_format = format;
}

char *GxmTexture::storage(void) const
{
	return m_storage->address();
}
