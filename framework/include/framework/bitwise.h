/*
 * bitwise.h - utility bitwise functionality
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BITWISE_H
#define BITWISE_H

template <typename T>
constexpr T clz(T val)
{
	T lz = 0;
	while (val >>= 1) { ++lz; }
	return sizeof(T)*8 - lz;
}

template <typename T>
constexpr bool isPow2(T v)
{
	return !(!v || (v & (v - 1)));
}

template <typename T>
constexpr T alignNearestPow2(T val)
{
	return isPow2(val) ? val : (1 << ((sizeof(val)*8) - clz(val) + 1));
}

template <typename T>
constexpr T alignPow2(T val, std::size_t align)
{
	return (val + align-1) & ~(align-1);
}

#endif // BITWISE_H
