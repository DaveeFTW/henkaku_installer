/*
 * worldentity.h - base class describing an entity within the world
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef WORLDENTITY_H
#define WORLDENTITY_H

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class WorldEntity
{
public:
	WorldEntity(void)
		: m_model(1)
		, m_world(1)
		, m_translate(1)
		, m_rotation(1)
		, m_scale(1)
		, m_internalTranslation(1)
		, m_internalRotation(1)
		, m_internalScale(1)
	{
		setScale(1.f);
		setRotation(0.f);
		setTranslation(0.f, 0.f, 0.f);
	}

	virtual ~WorldEntity(void) = default;

	void setScale(float scale) 
	{ 
		setScale(glm::vec3(scale, scale, scale));
	}

	void setScale(float x, float y, float z=1.f)
	{
		setScale(glm::vec3(x, y, z));
	}

	void setScale(glm::vec3 xyz)
	{
		m_scale = glm::scale(glm::mat4(1), xyz);
		updateModelMatrix();
	}

	void setRotation(float rotation)
	{
		m_rotation = glm::rotate(glm::mat4(1), glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f));
		updateModelMatrix();
	}

	void setTranslation(float x, float y, float z=0.f)
	{
		setTranslation(glm::vec3(x, y, z));
	}

	void setTranslation(glm::vec3 xyz)
	{
		m_translate = glm::translate(glm::mat4(1), xyz);
		updateModelMatrix();
	}

	void setWorldMatrix(glm::mat4 world)
	{
		m_world = world;
		updateModelMatrix();
	}

	virtual glm::mat4 modelMatrix(void) const
	{
		return m_model;
	}

protected:
	void setModel(glm::mat4 translation, glm::mat4 rotation, glm::mat4 scale)
	{
		m_internalTranslation = translation;
		m_internalRotation = rotation;
		m_internalScale = scale;
		updateModelMatrix();
	}

	virtual void onModelChanged(glm::mat4 model) { }

private:
	void updateModelMatrix(void)
	{
		m_model = m_world * m_translate * m_internalTranslation * m_rotation * m_internalRotation * m_scale * m_internalScale;
		onModelChanged(m_model);
	}

private:
	glm::mat4 m_model, m_world, m_translate, m_rotation, m_scale, m_internalTranslation, m_internalRotation, m_internalScale;
};

#endif // WORLDENTITY_H
