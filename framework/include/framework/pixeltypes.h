/*
 * pixeltypes.h - various pixel formats
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef PIXELTYPES_H
#define PIXELTYPES_H

#include <framework/pixeltypes.h>
#include <cstdint>

namespace framework
{
	using R8G8B8A8 = union alignas(4)
	{
		std::uint32_t rgba;
		
		struct 
		{
			std::uint8_t r;
			std::uint8_t g;
			std::uint8_t b;
			std::uint8_t a;
		} components;
	};
} // framework namespace

#endif // PIXELTYPES_H
