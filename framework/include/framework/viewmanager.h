/*

Copyright (C) 2015, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.


 */

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <vector>
#include <memory>

class View;
using ViewPtr = std::shared_ptr<View>;
using ViewList = std::vector<ViewPtr>;

class TaskScheduler;

class ViewManager
{
public:
	ViewManager(void);
	~ViewManager(void);

	void addView(ViewPtr view);
	void removeView(ViewPtr view);

	void exec(void);

private:
	ViewList m_views;
	std::unique_ptr<TaskScheduler> m_taskScheduler;
};

#endif // VIEWMANAGER_H
