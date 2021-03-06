#pragma once
#include "vector3.h"
using namespace xLib;
// this function is mesure how fast the length of z growth while in iteration. 
// it's a numerical calculation of z.length()'s differential

template<typename T>
float potential(T& pos) restrict(amp, cpu)
{
	T z = pos;
	float Power = 8.0f;
	int Iterations = 100;
	float minR = 1000.0f;
	for (int i = 1; i < Iterations; i++)
	{
		float r = z.Length();
		float theta = acos(z.z / r);
		float phi = atan(z.y / z.x);
		float zr = pow(r, Power);
		theta = theta*Power;
		phi = phi*Power;

		z = T(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta)) * zr;
		z = pos + z;
		float rt = z.Length();
		if (rt < minR)
		{
			minR = rt;
		}
		if (z * z > 1000.0f)
		{
			pos.w = minR * 2.0f;
			return log(rt) / pow(Power, float(i));
		}
	}
	return 0.0f;

}

// using de function... actually not very understand that function yet.
// can refer to original doc: Hypercomplex Iterations, Distance Estimation and Higher Dimensional Fractals. Theorem 2.

template <typename T>
float DE(T& point) restrict(amp, cpu)
{
	float EPS = 0.001f;
	float pot = potential(point);
	if (pot == 0.0f) return 0.0f;
	T gradient = (T(potential(point + T(1.0f, 0.0f, 0.0f) * EPS), potential(point + T(0.0f, 1.0f, 0.0f)*EPS), potential(point + T(0.0f, 0.0f, 1.0f)*EPS)) - T(pot, pot, pot)) * (1 / EPS);
	return (0.5f / exp(pot))*sinh(pot) / gradient.Length();
}


Vector3<float> GetNormal(Vector3<float> point, float eps) restrict(amp)
{
	eps = eps * 5.0f;
	Vector3<float> normal = Vector3<float>(DE(point + Vector3<float>(eps, 0, 0)) - DE(point - Vector3<float>(eps, 0, 0)),
		DE(point + Vector3<float>(0, eps, 0)) - DE(point - Vector3<float>(0, eps, 0)),
		DE(point + Vector3<float>(0, 0, eps)) - DE(point - Vector3<float>(0, 0, eps))).Normalize();
	return normal;
}

Vector3<float> GetColor(Vector3<float> point) restrict(amp)
{
	float t = point.w - int(point.w);
	float r = 9.0f * (1.0f - t)*t*t*t;
	float g = 15.0f * (1.0f - t)*(1.0f - t)*t*t;
	float b = 8.5f*(1.0f - t)*(1.0f - t)*(1 - t)*t;
	return Vector3<float>(r, g, b);
}