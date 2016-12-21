/*
 * camera.h - facility for providing a camera in a 3d scene
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
 
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
	Camera(void);

	float fieldOfView(void) const;
	void setFieldOfView(float fov);

	float aspectRatio(void) const;
	void setAspectRatio(float aspectRatio);

	float left(void) const;
	void setLeft(float left);

	float right(void) const;
	void setRight(float right);

	float bottom(void) const;
	void setBottom(float bottom);

	float top(void) const;
	void setTop(float top);

	float near(void) const;
	void setNear(float near);

	float far(void) const;
	void setFar(float far);

	glm::vec3 position(void) const;
	void setPosition(const glm::vec3& position);

	glm::vec3 upVector(void) const;
	void setUpVector(const glm::vec3& upVector);

	glm::vec3 viewCenter(void) const;
	void setViewCenter(const glm::vec3& viewCenter);

	void setPerspectiveProjection(float fov, float aspectRatio, float near, float far);
	void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);

	glm::mat4 projectionMatrix(void) const;
	glm::mat4 viewMatrix(void) const;

private:
	enum ProjectionTypes
	{
		PerspectiveProjection,
		OrthographicProjection,
	};
	
	void updateProjectionMatrix(void);
	void updateViewMatrix(void);

private:
	glm::vec3 m_position;
	glm::vec3 m_upVector;
	glm::vec3 m_viewCenter;
	glm::mat4 m_projection;
	glm::mat4 m_viewMatrix;
	float m_fov;
	float m_aspectRatio;
	float m_left;
	float m_right;
	float m_bottom;
	float m_top;
	float m_near;
	float m_far;
	ProjectionTypes m_projectionType;
};

#endif // CAMERA_H
