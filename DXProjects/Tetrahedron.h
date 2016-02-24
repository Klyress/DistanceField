#pragma once
#include "vector3.h"
using namespace xLib;

float DE(Vector3<float> point) restrict(amp, cpu)
{
	Vector3<float> a1 = Vector3<float>(1, 1, 1);
	Vector3<float> a2 = Vector3<float>(-1, -1, 1);
	Vector3<float> a3 = Vector3<float>(1, -1, -1);
	Vector3<float> a4 = Vector3<float>(-1, 1, -1);
	Vector3<float> c;
	float scale = 2.0f;
	int n = 0;
	float dist, d;
	while (n < 50) 
	{
		c = a1; 
		dist = (point - a1).Length();
		d = (point - a2).Length(); if (d < dist) { c = a2; dist = d; }
		d = (point - a3).Length(); if (d < dist) { c = a3; dist = d; }
		d = (point - a4).Length(); if (d < dist) { c = a4; dist = d; }
		point = point*scale - c*(scale - 1.0f);
		n++;
	}

	return point.Length() * pow(scale, float(-n));
}

Vector3<float> GetNormal(Vector3<float> point, float eps) restrict(amp)
{
	Vector3<float> normal = Vector3<float>(DE(point + Vector3<float>(eps, 0, 0)) - DE(point - Vector3<float>(eps, 0, 0)),
		DE(point + Vector3<float>(0, eps, 0)) - DE(point - Vector3<float>(0, eps, 0)),
		DE(point + Vector3<float>(0, 0, eps)) - DE(point - Vector3<float>(0, 0, eps))).Normalize();
	return normal;
}