/*
 * gxmtexture.h - encapsulate a single gxm texture
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GXMTEXTURE_H
#define GXMTEXTURE_H

#include <framework/gpumemoryblock.h>

#include <memory>
#include <cstdint>

struct SceGxmTexture;
struct SceGxmContext;

class GxmTexture
{
public:
	enum CoordinateDirection
	{
		DirectionU,
		DirectionV
	};

	enum WrapMode
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		MirrorClamp,
		RepeatIgnoreBorder,
		ClampFullBorder,
		ClampIgnoreBorder,
		ClampHalfBorder
	};

	enum Filter
	{
		Point,
		Linear
	};

	enum class MipFilter
	{
		Disabled,
		Enabled
	};

	enum TextureFormat
	{
		ARGB8,
		U8_R111 // set U8 as alpha channel and BGR to 111
	};

public:
	GxmTexture(void);
	~GxmTexture(void) = default;

	void allocateStorage(void);
	bool isStorageAllocated(void) const;

	void bind(SceGxmContext *ctx, std::uint32_t unit) const;
	void setSize(std::size_t width, std::size_t height = 1, std::size_t depth = 1);

	std::size_t width(void) const { return m_width; }
	std::size_t height(void) const { return m_height; }
	std::size_t depth(void) const { return m_depth; }

	void setMinificationFilter(Filter filter);
	void setMagnificationFilter(Filter filter);
	void setMinMagFilter(Filter min, Filter mag);
	void setWrapMode(CoordinateDirection direction, WrapMode mode);
	void setWrapMode(WrapMode mode);

	void setFormat(TextureFormat format);

	void setData(const void *data);
	void setEmptyData(void);

protected:
	char *storage(void) const;

private:
	std::unique_ptr<SceGxmTexture> m_texture;
	std::unique_ptr<GpuMemoryBlock<char>> m_storage;
	std::size_t m_width{0};
	std::size_t m_height{1};
	std::size_t m_depth{1};
	TextureFormat m_format;
};

#endif //GXMTEXTURE_H
