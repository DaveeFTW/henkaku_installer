/*
 * easingcurves.h - various easing curves
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef EASINGCURVES_H
#define EASINGCURVES_H

namespace easing
{
	namespace details
	{
		constexpr auto pi = 3.14159265358979323846;

		template <typename T, typename F>
		T inOut(T t, T b, T c, T d, F in, F out)
		{
			return (t/(d/2) < 1) ? in(t, b, c, d) : out(t, b, c, d);
		}
	} // namespace details

	template <typename T>
	T linear(T t, T b, T c, T d)
	{
		return c*t/d + b;
	}

	namespace quad
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			t = t/d;
			return c*t*t + b;
		}

		template <typename T>
		T out(T t, T b, T c, T d)
		{
			t = t/d;
			return -c*t*(t-2) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace quad

	namespace cubic
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			t /= d;
			return c*t*t*t + b;
		}

		template <typename T>
		T out(T t, T b, T c, T d)
		{
			t /= d;
			t--;
			return c*(t*t*t + 1) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace cubic

	namespace quart
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			t /= d;
			return c*t*t*t*t + b;
		}
		
		template <typename T>
		T out(T t, T b, T c, T d)
		{
			t /= d;
			t--;
			return -c * (t*t*t*t - 1) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace quart

	namespace quint
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			t /= d;
			return c*t*t*t*t*t + b;
		}
		
		template <typename T>
		T out(T t, T b, T c, T d)
		{
			t /= d;
			t--;
			return c*(t*t*t*t*t + 1) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace quint

	namespace sine
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			return -c * std::cos(t/d * (details::pi/2)) + c + b;
		}
		
		template <typename T>
		T out(T t, T b, T c, T d)
		{
			return c * std::sin(t/d * (details::pi/2)) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace sine

	namespace exponential
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			return c * std::pow( 2, 10 * (t/d - 1) ) + b;
		}
		
		template <typename T>
		T out(T t, T b, T c, T d)
		{
			return c * ( -std::pow( 2, -10 * t/d ) + 1 ) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace exponential

	namespace circular
	{
		template <typename T>
		T in(T t, T b, T c, T d)
		{
			t /= d;
			return -c * (std::sqrt(1 - t*t) - 1) + b;
		}
		
		template <typename T>
		T out(T t, T b, T c, T d)
		{
			t /= d;
			t--;
			return c * std::sqrt(1 - t*t) + b;
		}

		template <typename T>
		T inOut(T t, T b, T c, T d)
		{
			return details::inOut(t, b, c, d, in<T>, out<T>);
		}
	} // namespace exponential
} // namespace easing

#endif // EASINGCURVES_H
