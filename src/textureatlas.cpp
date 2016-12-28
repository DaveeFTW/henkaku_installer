/*
 * textureatlas.cpp - texture atlas based on freetype-gl
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.
 * Copyright 2011-2016 Nicolas P. Rougier
 * Copyright 2013-2016 Marcel Metz
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.

 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the freetype-gl project.
 */

#include "textureatlas.h"

#include <limits>
#include <cstring>

TextureAtlas::TextureAtlas(void)
{
	
}

TextureAtlas::TextureAtlas(std::size_t width, std::size_t height)
{
	setSize(width, height);
}

void TextureAtlas::setWidth(std::size_t width)
{
	setSize(width, height());
}

void TextureAtlas::setHeight(std::size_t height)
{
	setSize(width(), height);
}

void TextureAtlas::create(GxmTexture::Filter minFilter, GxmTexture::Filter magFilter)
{
	allocateStorage();
	setEmptyData();
	setMinMagFilter(minFilter, magFilter);
	m_nodes.push_back(glm::ivec3(1, 1, width()-2));
}

void TextureAtlas::setRegion(AtlasRegion region, const char *data, std::size_t stride)
{
	auto x = region.x;
	auto y = region.y;
	auto width = region.z-1;
	auto height = region.w-1;
	
	for (auto i = 0; i < height; ++i)
	{
		std::memcpy(storage() + ((y+i)*this->width() + x), data + (i*stride), width);
	}
}

TextureAtlas::AtlasRegion TextureAtlas::region(std::size_t width, std::size_t height)
{
	constexpr auto max_size = std::numeric_limits<std::size_t>::max();
	std::size_t best_height = max_size;
	std::size_t best_width = max_size;

	// we want a border around our region
	width += 1;
	height += 1;

	auto region = AtlasRegion(0, 0, width, height);
	auto best_node = m_nodes.end();
	
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		auto y = fit(it, width, height);
		
		if (y >= 0)
		{
			auto node = *it;
			
			if (((y + height) < best_height)
				|| (((y + height) == best_height) && (node.z > 0 && static_cast<std::size_t>(node.z) < best_width)))
			{
				best_height = y + height;
				best_width = node.z;
				best_node = it;
				region.x = node.x;
				region.y = y;
			}
		}
	}

	if (best_node == m_nodes.end())
	{
		region.x = -1;
		region.y = -1;
		region.z = 0;
		region.w = 0;
		return region;
	}

	m_nodes.insert(best_node, glm::ivec3(region.x, region.y + height, width));
	for (auto it = best_node; it != m_nodes.end();)
	{
		auto node = it;
		auto prev = std::prev(it);
		
		if (node->x < (prev->x + prev->z))
		{
			int shrink = prev->x + prev->z - node->x;
			node->x += shrink;
			node->z -= shrink;
			
			if (node->z <= 0)
			{
				it = m_nodes.erase(it);
			}
			else
			{
				++it;
				break;
			}
		}
		else
		{
			++it;
			break;
		}
	}
	
	merge();
	return region;
}

TextureAtlas::Quad TextureAtlas::toQuad(AtlasRegion region)
{
	Quad quad;

	auto x = static_cast<float>(region.x);
	auto y = static_cast<float>(region.y);
	auto width = static_cast<float>(region.z);
	auto height = static_cast<float>(region.w);
	auto texWidth = static_cast<float>(this->width());
	auto texHeight = static_cast<float>(this->height());
	auto glyph = glm::vec4(x/texWidth, y/texHeight, width/texWidth, height/texHeight);
	
	quad.tl = glm::vec2(glyph.x, glyph.y);
	quad.tr = glm::vec2(glyph.x+glyph.z, glyph.y);
	quad.bl = glm::vec2(glyph.x, glyph.y+glyph.w);
	quad.br = glm::vec2(glyph.x+glyph.z, glyph.y+glyph.w);
	quad.size = glm::vec2(width, height);
	return quad;
}

int TextureAtlas::fit(NodeList::iterator it, std::size_t width, std::size_t height)
{
	auto node = *it;
	auto x = node.x;
	auto y = node.y;
	int width_left = width;

	if ((x + width) > (this->width()-1))
	{
		return -1;
	}

	while (width_left > 0)
	{
		node = *it;
		
		if (node.y > y)
		{
			y = node.y;
		}
		
		if ((y + height) > this->height()-1)
		{
			return -1;
		}

		width_left -= node.z;
		++it;
	}

	return y;
}

void TextureAtlas::merge(void)
{
	for (auto it = m_nodes.begin(); it != m_nodes.end();)
	{
		auto node = it;
		auto next = std::next(it);

		if (next == m_nodes.end())
			return;

		if (node->y == next->y)
		{
			node->z += next->z;
			m_nodes.erase(next);
		}
		else
		{
			++it;
		}
	}
}
