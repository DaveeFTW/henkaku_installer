/*
 * geometry.h - define simple base class for geometry
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "worldentity.h"

#include <functional>

struct SceGxmContext;

class Camera;
class GeometryRenderer;

class Geometry : public WorldEntity
{
	using FragmentTask = std::function<void(SceGxmContext *ctx)>;
	
public:
	virtual ~Geometry(void) = default;

	void setFragmentTask(FragmentTask task)
	{
		m_fragmentTask = task;
	}

	void draw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const
	{
		if (m_fragmentTask)
		{
			m_fragmentTask(ctx);
		}

		return doDraw(ctx, renderer, camera);
	}

private:
	virtual void doDraw(SceGxmContext *ctx, const GeometryRenderer *renderer, const Camera *camera) const = 0;

private:
	FragmentTask m_fragmentTask;
};

#endif // GEOMETRY_H
