#pragma once
#include "vector3.h"
using namespace xLib;

float DE(Vector3<float>& point) restrict(amp, cpu)
{
	Vector3<float> c = Vector3<float>(1.0f, 1.0f, 1.0f);
	const float foldL = 1.0f;
	const float foldR = 1.0f;
	const float foldRMin = 0.5f;
	float scale = 2.0f;
	Vector3<float> z = point;
	float DEFactor = scale;
	//float m = 1.0f;
	float minR = 1000.0f;

	int Iterations = 30;
	for (int i = 1; i < Iterations; i++)
	{
		float m = 1.0f;

		if (z.x > foldL)
		{
			z.x = 2.0f * foldL - z.x;
		}
		else if (z.x < -foldL)
		{
			z.x = -2.0f * foldL - z.x;
		}

		if (z.y > foldL) { z.y = 2.0f * foldL - z.y; }
		else if (z.y < -foldL) { z.y = -2.0f * foldL - z.y; }
		if (z.z > foldL) { z.z = 2.0f * foldL - z.z; }
		else if (z.z < -foldL) { z.z = -2.0f * foldL - z.z; }

		float r2 = z * z;
		if (r2 < foldRMin * foldRMin)
		{
			m = ((foldR * foldR) / (foldRMin * foldRMin));
			z = z * m;
			DEFactor = DEFactor * m;
		}
		else if (r2 < foldR * foldR)
		{
			m = ((foldR * foldR) / (r2 * r2));
			z = z * m;
			DEFactor = DEFactor * m;
		}

		z = z * scale + point;
		float r = z.Length();
		if (r < minR)
		{
			minR = r;
		}
		DEFactor = DEFactor * scale + 1.0f;
	}
	if (DEFactor < 0.0f)
	{
		DEFactor = -DEFactor;
	}
	point.w = minR / 3.0f;
	return z.Length() / DEFactor;
}

Vector3<float> GetNormal(Vector3<float> point, float eps) restrict(amp)
{
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