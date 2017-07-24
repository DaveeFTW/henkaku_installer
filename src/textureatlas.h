#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include <framework/gxmtexture.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <list>

class TextureAtlas : private GxmTexture
{
public:
	using AtlasRegion = glm::ivec4;
	struct Quad
	{
		glm::vec2 bl;
		glm::vec2 br;
		glm::vec2 tr;
		glm::vec2 tl;
		glm::vec2 size;
	};
	
public:
	TextureAtlas(void);
	TextureAtlas(std::size_t width, std::size_t height);

	void setWidth(std::size_t width);
	using GxmTexture::width;
	
	void setHeight(std::size_t height);
	using GxmTexture::height;

	using GxmTexture::setFormat;
	using GxmTexture::bind;

	void create(GxmTexture::Filter minFilter, GxmTexture::Filter magFilter);

	AtlasRegion region(std::size_t width, std::size_t height);
	void setRegion(AtlasRegion region, const char *data, std::size_t stride);

	Quad toQuad(AtlasRegion region);

private:
	using NodeList = std::list<glm::ivec3>;
	int fit(NodeList::iterator it, std::size_t width, std::size_t height);
	void merge(void);

private:
	NodeList m_nodes;
};

#endif // TEXTUREATLAS_H
