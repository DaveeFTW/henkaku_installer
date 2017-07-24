/*
 * checkbox.h - simple checkbox implementation
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "worldentity.h"
#include "vertextypes.h"
#include "texturerectangle.h"
#include "numberanimation.h"

class GeometryRenderer;
class Text;

class CheckBox : public WorldEntity
{
public:
	CheckBox(GeometryRenderer *geometryRenderer, GeometryRenderer *textRenderer);

	void setText(Text *text);

	void setChecked(bool checked);
	bool checked(void) const;
	void toggle(void);

	float width(void) const;
	float height(void) const;

	void update(float dt);
	void draw(SceGxmContext *ctx, const Camera *camera) const;

private:
	void onModelChanged(glm::mat4 model) final;
	void positionComponents(void);

private:
	GeometryRenderer *m_geometryRenderer;
	GeometryRenderer *m_textRenderer;
	bool m_checked;
	TextureRectangle<ColouredTextureVertex> m_checkboxSelected, m_checkboxUnselected;
	Text *m_text;
	float m_width;
	GxmTexture m_checkedTexture, m_uncheckedTexture;
	NumberAnimation m_checkedAnimation;
};

#endif // CHECKBOX_H
