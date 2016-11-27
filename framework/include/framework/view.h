/*
 * view.h - base class for a view surface
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VIEW_H
#define VIEW_H

#include <Eigen/Dense>

class View
{
	friend class ViewManager;

public:
	View(void);
	virtual ~View(void) = default;

	Eigen::Matrix4f model(void);

	virtual void setOpacity(float opacity) = 0;
	virtual float opacity(void) = 0;

protected:
	virtual void update(double dt) = 0;
	virtual void render(void) = 0;

private:
	Eigen::Matrix4f m_matrix;
};

#endif // VIEW_H