#pragma once

#include "vector3.h"
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
using namespace concurrency;
using namespace concurrency::graphics;
using namespace concurrency::graphics::direct3d;
using namespace concurrency::direct3d;
using namespace concurrency::fast_math;

#ifdef USING_GPU
#define _GPU restrict(amp, cpu)
#else
#define _GPU
#endif

namespace xLib
{
	template<typename T>
	class quaternion
	{
	public:
		quaternion() _GPU
		{
			x = y = z = w = 1.0f;
		}
		quaternion(const T x, const T y, const T z, const T w) _GPU
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		quaternion(const Vector3<T>& axis, const T angle) _GPU
		{
			x = axis.x * sin(angle / 2.0f);
			y = axis.y * sin(angle / 2.0f);
			z = axis.z * sin(angle / 2.0f);
			w = cos(angle / 2.0f);
		}
		quaternion(const quaternion& a) _GPU
		{
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
		};

		Vector3<T> Rotate(const Vector3<T>& v) _GPU
		{
			Vector3<T> result;
			Vector3<T> u(x, y, z);
			float s = w;

			result = u * (u * v) * 2.0f
				+  v * (s*s - u * u)
				+ u.Cross(v) * s * 2.0f;
			return result;
		}

		quaternion<T> operator*(const quaternion<T>& q) _GPU
		{
			quaternion<T> result;
			result.x = x*q.x - y*q.y - z*q.z - w*q.w;
			result.y = x*q.y + y*q.x - z*q.w + w*q.z;
			result.z = x*q.z + y*q.w + z*q.x - w*q.y;
			result.w = x*q.w - y*q.z + z*q.y + w*q.x;
			return result;
		}

		quaternion<T> operator+(const quaternion<T>& q) _GPU
		{
			quaternion<T> result;
			result.x = x + q.x;
			result.y = y + q.y;
			result.z = z + q.z;
			result.w = w + q.w;
			return result;
		}

		quaternion<T> operator*(const float s) _GPU
		{
			quaternion<T> result;
			result.x = x * s;
			result.y = y * s;
			result.z = z * s;
			result.w = w * s;
			return result;
		}

		void operator=(const quaternion<T>& q) _GPU
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
		}

		T Length() _GPU
		{
			return sqrt(x*x + y*y + z*z + w*w);
		}
		T x, y, z, w;
	};
};