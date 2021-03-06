/*
 * camera.cpp - facility for providing a camera in a 3d scene
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
 
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(void)
	: m_position(0, 0, 0)
	, m_upVector(0, 1, 0)
	, m_viewCenter(0, 0, 0)
	, m_fov(45.f)
	, m_aspectRatio(1.f)
	, m_left(-1.f)
	, m_right(1.f)
	, m_bottom(-1.f)
	, m_top(1.f)
	, m_near(0.f)
	, m_far(1.f)
	, m_projectionType(PerspectiveProjection)
{
	// set default projection matrix
	updateProjectionMatrix();
	updateViewMatrix();
}

float Camera::fieldOfView(void) const
{
	return m_fov;
}

void Camera::setFieldOfView(float fov)
{
	m_fov = fov;
	updateProjectionMatrix();
}

float Camera::aspectRatio(void) const
{
	return m_aspectRatio;
}

void Camera::setAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	updateProjectionMatrix();
}

float Camera::left(void) const
{
	return m_left;
}

void Camera::setLeft(float left)
{
	m_left = left;
}

float Camera::right(void) const
{
	return m_right;
}

void Camera::setRight(float right)
{
	m_right = right;
}

float Camera::bottom(void) const
{
	return m_bottom;
}

void Camera::setBottom(float bottom)
{
	m_bottom = bottom;
}

float Camera::top(void) const
{
	return m_top;
}

void Camera::setTop(float top)
{
	m_top = top;
}

float Camera::near(void) const
{
	return m_near;
}
void Camera::setNear(float near)
{
	m_near = near;
	updateProjectionMatrix();
}

float Camera::far(void) const
{
	return m_far;
}

void Camera::setFar(float far)
{
	m_far = far;
	updateProjectionMatrix();
}

glm::vec3 Camera::position(void) const
{
	return m_position;
}

void Camera::setPosition(const glm::vec3& position)
{
	m_position = position;
	updateViewMatrix();
}

glm::vec3 Camera::upVector(void) const
{
	return m_upVector;
}

void Camera::setUpVector(const glm::vec3& upVector)
{
	m_upVector = upVector;
	updateViewMatrix();
}


glm::vec3 Camera::viewCenter(void) const
{
	return m_viewCenter;
}

void Camera::setViewCenter(const glm::vec3& viewCenter)
{
	m_viewCenter = viewCenter;
	updateViewMatrix();
}

void Camera::setPerspectiveProjection(float fov, float aspectRatio, float near, float far)
{
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_near = near;
	m_far = far;
	m_projectionType = PerspectiveProjection;
	updateProjectionMatrix();
}

void Camera::setOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
{
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
	m_near = near;
	m_far = far;
	m_projectionType = OrthographicProjection;
	updateProjectionMatrix();
}

glm::mat4 Camera::projectionMatrix(void) const
{
	return m_projection;
}

glm::mat4 Camera::viewMatrix(void) const
{
	return m_viewMatrix;
}

void Camera::updateProjectionMatrix(void)
{
	switch (m_projectionType)
	{
	case PerspectiveProjection:
		m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
		break;
	case OrthographicProjection:
		m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
		break;
	}
}

void Camera::updateViewMatrix(void)
{
	m_viewMatrix = glm::lookAt(m_position, m_viewCenter, m_upVector);
}
