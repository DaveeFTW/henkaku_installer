/*
 * input.h - abstract class for creating input events
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef INPUT_H
#define INPUT_H

#include <memory>
#include <vector>

class Task;
using TaskPtr = std::shared_ptr<Task>;

class Input
{
public:
	virtual std::vector<TaskPtr> tasks(void) = 0;
};

#endif // INPUT_H
