#pragma once
#include "vector3.h"
using namespace xLib;

float DE(Vector3<float> point) restrict(amp, cpu)
{
	int i = 0;
	Vector3<float> z = point;
	float t;
	int iteration = 4;
	while (i < iteration)
	{	
		if (z.x < 0.0f)	z.x = -z.x;
		if (z.y < 0.0f)	z.y = -z.y;
		if (z.z < 0.0f)	z.z = -z.z;
		if (z.x < z.y)
		{
			t = z.x;
			z.x = z.y;
			z.y = t;
		}
		if (z.x < z.z)
		{
			t = z.x;
			z.x = z.z;
			z.z = t;
		}
		if (z.y < z.z)
		{
			t = z.z;
			z.z = z.y;
			z.y = t;
		}
		z = z * 3.0f - Vector3<float>(2.0f, 2.0f, 2.0f);

		if (z.z < -1.0f)
		{
			z.z += 2.0f;
		}
		i++;
	}

	if (z.x < 0.0f)	z.x = -z.x;
	if (z.y < 0.0f)	z.y = -z.y;
	if (z.z < 0.0f)	z.z = -z.z;
	
	z = z - Vector3<float>(1.0f, 1.0f, 1.0f);
	Vector3<float> p = z;
	if (p.x < 0.0f) p.x = 0.0f;
	if (p.y < 0.0f) p.y = 0.0f;
	if (p.z < 0.0f) p.z = 0.0f;
	float dis = min(max(z.x, max(z.y, z.z)), 0.0f) + p.Length();

	//scale cube size to iterations
	return min(dis * pow(3.0f, -iteration), point.y + 1.0f);
}

Vector3<float> GetNormal(Vector3<float> point, float eps) restrict(amp)
{
	Vector3<float> normal = Vector3<float>(DE(point + Vector3<float>(eps, 0, 0)) - DE(point - Vector3<float>(eps, 0, 0)),
		DE(point + Vector3<float>(0, eps, 0)) - DE(point - Vector3<float>(0, eps, 0)),
		DE(point + Vector3<float>(0, 0, eps)) - DE(point - Vector3<float>(0, 0, eps))).Normalize();
	return normal;
}