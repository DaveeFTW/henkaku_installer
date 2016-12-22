/*
 * installerview.h - main UI view
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/view.h>
#include <framework/gxmshaderpatcher.h>

#include <memory>

class AnimatedBackground;
class Camera;

class InstallerView : public View
{
public:
	InstallerView(void);
	~InstallerView(void);

	TaskPtr simulationTask(double dt) override;
	void render(SceGxmContext *ctx) override;

private:
	void update(void);

private:
	TaskPtr m_simulationTasks;
	GxmShaderPatcher m_patcher;
	AnimatedBackground *m_animatedBackground;
	Camera *m_camera;
	float m_dt;
};
