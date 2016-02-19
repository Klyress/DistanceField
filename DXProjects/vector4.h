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
	class Vector4
	{
	public:
		Vector4() _GPU
		{
			x = y = z = w = 0;
		};

		Vector4(T x, T y, T z, T w) _GPU
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		};
		Vector4(T* e) _GPU
		{
			x = e->x;
			y = e->y;
			z = e->z;
			w = e->w;
		};

		Vector4(const Vector3<T>& a) _GPU
		{
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
		};

		Vector4 operator+ (const Vector4<T>& a) const _GPU
		{
			return Vector4<T>(x + a.x, y + a.y, z + a.z, w + a.w);
		};
		Vector4 operator- (const Vector4<T>& a) const _GPU
		{
			return Vector4<T>(x - a.x, y - a.y, z - a.z, w - a.w);
		};
		Vector4 operator-() _GPU
		{
			return Vector4<T>(-x, -y, -z, -w);
		};
		// dot
		float operator* (const Vector4<T>& a) const _GPU
		{
			return x*a.x + y*a.y + z*a.z + w*a.w;
		};
		// scale
		Vector4 operator* (const T a) const _GPU
		{
			return Vector4<T>(x*a, y*a, z*a, w*a);
		};
		friend Vector4 operator*(const T a, const Vector4<T> v)
		{
			return Vector4<T>(x*a, y*a, z*a, w*a);
		}
		Vector4 operator= (const Vector4<T>& a) _GPU
		{
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
			return (*this);
		};
		bool equal(const Vector4<T>& a) const _GPU
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
			case 3:return w;
			}
		};

		bool isZero() const _GPU
		{
			return x == 0 && y == 0 && z == 0 && w == 0;
		};

		float Length() _GPU
		{
			return sqrt(x*x + y*y + z*z + w*w);
		};

		Vector4<T> Normalize() _GPU
		{
			float length = Length();
			x = x / length;
			y = y / length;
			z = z / length;
			w = z / length;
			return *this;
		};


	public:
		T x, y, z, w;
	};

};

