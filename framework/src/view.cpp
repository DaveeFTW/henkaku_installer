/*
 * view.cpp - base class for a view surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/view.h>

View::View(void)
	: m_matrix(Eigen::Matrix4f::Identity())
{
}

Eigen::Matrix4f View::model(void)
{
	return m_matrix;
}
