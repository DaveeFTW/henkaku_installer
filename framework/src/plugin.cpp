/*
 * plugin.cpp - minimal implementation of entry points to compile as a plugin
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

extern "C"
{
	int module_start(void *dst, void *src, int len)
	{
		return 0;
	}

	void _start() __attribute__ ((weak, alias ("module_start")));
} // extern C
