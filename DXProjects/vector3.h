
#pragma once
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
	template <typename T>
	class Vector3
	{
	public:
		Vector3() _GPU
		{
			x = y = z = 0;
		};

		Vector3(T x, T y, T z) _GPU
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = 1.0f;
		};
		Vector3(T* e) _GPU
		{
			x = e[0];
			y = e[1];
			z = e[2];
			w = e[3];
		};

		Vector3(const Vector3<T>& a) _GPU
		{
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
		};

		Vector3 operator+ (const Vector3<T>& a) const _GPU
		{
			return Vector3<T>(x + a.x, y + a.y, z + a.z);
		};
		Vector3 operator- (const Vector3<T>& a) const _GPU
		{
			return Vector3<T>(x - a.x, y - a.y, z - a.z);
		};
		Vector3 operator-() _GPU
		{
			return Vector3<T>(-x, -y, -z);
		};
		float operator* (const Vector3<T>& a) const _GPU
		{
			return x*a.x + y*a.y + z*a.z;
		};
		Vector3 operator* (const T a) const _GPU
		{
			return Vector3<T>(x*a, y*a, z*a);
		};

		Vector3 operator/ (const T a) const _GPU
		{
			return Vector3<T>(x/a, y/a, z/a);
		};
		Vector3 operator= (const Vector3<T>& a) _GPU
		{
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
			return (*this);
		};
		bool equal(const Vector3<T>& a) const _GPU
		{
			return ((*this) - a).isZero();
		};

		T& operator[] (const int index) _GPU
		{
			switch (index)
			{
			case 0:return x;
			case 1:return y;
			case 2:return z;
			}
		};

		Vector3<T> Cross(const Vector3<T>& a) const _GPU
		{
			Vector3<T> result;
			result.x = y * a.z - z * a.y;
			result.y = z * a.x - x * a.z;
			result.z = x * a.y - y * a.x;
			return result;
		};
		static Vector3<T> Cross(const Vector3<T>& a, const Vector3<T>& b) _GPU
		{
			Vector3<T> result;
			result = a.Cross(b);
			return result;
		};
		bool isZero() const _GPU
		{
			return x == 0 && y == 0 && z == 0;
		};

		float Length() _GPU
		{
			return sqrt(x*x + y*y + z*z);
		};
		Vector3<T> Normalize() _GPU
		{
			float length = Length();
			x = x / length;
			y = y / length;
			z = z / length;
			return *this;
		};

		Vector3<T> Reflect(Vector3<T> normal) _GPU
		{
			return (*this - normal * (*this * normal * 2.0f));
		}

		// Utility for type cast to other API
#ifdef USING_GPU
		inline operator float3() _GPU
		{
			return float3(x, y, z);
		};
#endif

	public:
		T x, y, z, w;
	};

	template <typename T>
	class Ray
	{
	public:
		Vector3<T> from;
		Vector3<T> to;
		Vector3<T> dir;
		float energy;

	public:
		Vector3<T> getDirection() restrict(cpu, amp)
		{
			dir = (to - from).Normalize();
			return dir;
		}
	};
};

