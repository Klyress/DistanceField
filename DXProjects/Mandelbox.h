#pragma once
#include "vector3.h"
using namespace xLib;

float MBDE(Vector3<float> point) restrict(amp, cpu)
{
	Vector3<float> c = Vector3<float>(1.0f, 1.0f, 1.0f);
	const float foldL = 1.0f;
	const float foldR = 1.0f;
	const float foldRMin = 0.5f;
	float scale = 2.8f;
	Vector3<float> z = point;
	float DEFactor = scale;
	//float m = 1.0f;

	int Iterations = 12;
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
		DEFactor = DEFactor * scale + 1.0f;
		//if (r2 > 100000.0f)
		//{
		//break;
		//}
	}
	if (DEFactor < 0.0f)
	{
		DEFactor = -DEFactor;
	}
	return z.Length() / DEFactor;
}

Vector3<float> GetNormal(Vector3<float> point) restrict(amp)
{
	Vector3<float> normal = Vector3<float>(MBDE(point + Vector3<float>(0.001f, 0, 0)) - MBDE(point - Vector3<float>(0.001f, 0, 0)),
		MBDE(point + Vector3<float>(0, 0.001f, 0)) - MBDE(point - Vector3<float>(0, 0.001f, 0)),
		MBDE(point + Vector3<float>(0, 0, 0.001f)) - MBDE(point - Vector3<float>(0, 0, 0.001f))).Normalize();
	return normal;
}