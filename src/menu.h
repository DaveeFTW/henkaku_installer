/*
 * menu.h - simple list menu
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef MENU_H
#define MENU_H

#include "worldentity.h"
#include "rectangle.h"
#include "vertextypes.h"
#include "numberanimation.h"

#include <stateless++/state_machine.hpp>

#include <vector>

class GeometryRenderer;
class Text;

class Menu : public WorldEntity
{
public:
	struct Item
	{
		Text *title;
		Text *desc;
	};

public:
	Menu(GeometryRenderer *geometryRenderer, GeometryRenderer *textRenderer);

	int add(Item item);
	int size(void) const;

	float width(void) const;
	float height(void) const;

	void up(void);
	void down(void);

	int selection(void) const;

	void setSelectionWidth(float width, float xoffset = 0);

	void update(float dt);
	void draw(SceGxmContext *ctx, const Camera *camera) const;

	void enableSelection();
	void disableSelection();

private:
	enum class Trigger
	{
		Up,
		Down
	};

	using StateMachine = stateless::state_machine<int, Trigger>;

private:
	void positionComponents(void);
	void positionSelectionBox(float y);
	void onModelChanged(glm::mat4 model) final;

private:
	std::vector<Item> m_items;
	Rectangle<ColouredGeometryVertex> m_selectionBox;
	GeometryRenderer *m_geometryRenderer;
	GeometryRenderer *m_textRenderer;
	StateMachine m_stateMachine;
	NumberAnimation m_selectionGlow;
	NumberAnimation m_selectionY;
	float m_width;
	float m_height;
	float m_componentHeight;
	float m_selectionBoxOffset;
	bool m_usingSelectedWidth;
	bool m_rising;
	bool m_selectionDisabled;
};

#endif // MENU_H
