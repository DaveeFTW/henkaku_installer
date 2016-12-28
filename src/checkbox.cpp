/*
 * checkbox.cpp - simple checkbox implementation
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "checkbox.h"
#include "geometryrenderer.h"
#include "text.h"
#include "resource.h"
#include "easingcurves.h"

#include <png++/png.hpp>

namespace
{
	void loadTexture(GxmTexture *texture, const char *file)
	{
		class VectorStream : public std::basic_streambuf<char, std::char_traits<char>>
		{
		public:
			VectorStream(std::vector<char>& v)
			{
				setg(v.data(), v.data(), v.data()+v.size());
			}
		};
		
		auto fileData = Resource::read(file);
		
		if (!fileData.good)
		{
			LOG(FATAL) << "could not load texture: "  << file;
		}

		VectorStream vstream(fileData.data);
		std::istream stream(&vstream);
		
		png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> image(stream);
		
		texture->setSize(image.get_width(), image.get_height());
		texture->setFormat(GxmTexture::ARGB8);
		texture->allocateStorage();
		texture->setData(image.get_pixbuf().get_bytes().data());
		texture->setMinMagFilter(GxmTexture::Linear, GxmTexture::Linear);
		texture->setWrapMode(GxmTexture::Repeat);
	}
}

CheckBox::CheckBox(GeometryRenderer *geometryRenderer, GeometryRenderer *textRenderer)
	: m_geometryRenderer(geometryRenderer)
	, m_textRenderer(textRenderer)
	, m_checked(true)
	, m_text(nullptr)
	, m_width(0.f)
{
	m_checkboxSelected.setWidth(50);
	m_checkboxSelected.setHeight(50);
	m_checkboxSelected.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));
	
	m_checkboxUnselected.setWidth(50);
	m_checkboxUnselected.setHeight(50);
	m_checkboxUnselected.setColour(glm::vec4(1.f, 1.f, 1.f, 1.f));

	loadTexture(&m_checkedTexture, "rsc:/textures/checkbox-checked.png");
	loadTexture(&m_uncheckedTexture, "rsc:/textures/checkbox-unchecked.png");

	m_checkboxSelected.setTexture(&m_checkedTexture);
	m_checkboxUnselected.setTexture(&m_uncheckedTexture);

	m_checkedAnimation.setDuration(200);
	m_checkedAnimation.setEasing([this](auto t, auto b, auto c, auto d)
	{
		using namespace easing::quad;
		return (this->m_checked) ? in(t, b, c, d) : out(t, b, c, d);
	});

	m_checkedAnimation.setStepHandler([this](auto step)
	{
		this->m_checkboxSelected.setColour(glm::vec4(1.f, 1.f, 1.f, step));
	});

	m_checkedAnimation.setCompletionHandler([this](void)
	{
		this->m_checked = !this->m_checked;
	});

	// we do this to ensure that the proper state is set
	m_checked = true;
	setChecked(!m_checked);
}

void CheckBox::setText(Text *text)
{
	m_text = text;
	positionComponents();
}

void CheckBox::setChecked(bool checked)
{
	if (checked != m_checked)
	{
		m_checkedAnimation.setStart(m_checked ? 1.f : 0.f);
		m_checkedAnimation.setEnd(m_checked ? -1.f : 1.f);
		m_checkedAnimation.start();
	}
}

bool CheckBox::checked(void) const
{
	return m_checked;
}

void CheckBox::setWidth(float width)
{
	m_width = width;
	positionComponents();
}

void CheckBox::onModelChanged(glm::mat4 model)
{
	if (m_text)
		m_text->setWorldMatrix(model);

	m_checkboxSelected.setWorldMatrix(model);
	m_checkboxUnselected.setWorldMatrix(model);
}

void CheckBox::positionComponents(void)
{
	if (!m_text)
		return;

	m_text->setTranslation(m_checkboxSelected.width()+((2.f * 960.f)/100.f), -m_text->height());

	m_checkboxSelected.setHeight(m_text->height());
	m_checkboxSelected.setWidth(m_text->height());
	m_checkboxSelected.setTranslation(0, -m_checkboxSelected.height());
	
	m_checkboxUnselected.setHeight(m_text->height());
	m_checkboxUnselected.setWidth(m_text->height());
	m_checkboxUnselected.setTranslation(0, -m_checkboxUnselected.height());
}

void CheckBox::update(float dt)
{
	m_checkedAnimation.update(dt);
}

void CheckBox::draw(SceGxmContext *ctx, const Camera *camera) const
{
	m_textRenderer->draw(ctx, camera, &m_checkboxSelected);
	m_textRenderer->draw(ctx, camera, &m_checkboxUnselected);
	m_textRenderer->draw(ctx, camera, m_text);
}
