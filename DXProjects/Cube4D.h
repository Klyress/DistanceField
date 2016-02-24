#pragma once
#include "vector3.h"
#include "vector4.h"

using namespace xLib;
float DE(Vector3<float> point) restrict(amp, cpu)
{
	// hyper cude has 16 vertices
	Vector4<float> pt[16] = {
		Vector4<float>(1.0f, 1.0f, 1.0f, 0.0f),
		Vector4<float>(-1.0f, 1.0f, 1.0f, 0.0f),
		Vector4<float>(-1.0f, -1.0f, 1.0f, 0.0f),
		Vector4<float>(1.0f, -1.0f, 1.0f, 0.0f),
		Vector4<float>(1.0f, 1.0f, -1.0f, 0.0f),
		Vector4<float>(-1.0f, 1.0f, -1.0f, 0.0f),
		Vector4<float>(-1.0f, -1.0f, -1.0f, 0.0f),
		Vector4<float>(1.0f, -1.0f, -1.0f, 0.0f),
		Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4<float>(-1.0f, 1.0f, 1.0f, 1.0f),
		Vector4<float>(-1.0f, -1.0f, 1.0f, 1.0f),
		Vector4<float>(1.0f, -1.0f, 1.0f, 1.0f),
		Vector4<float>(1.0f, 1.0f, -1.0f, 1.0f),
		Vector4<float>(-1.0f, 1.0f, -1.0f, 1.0f),
		Vector4<float>(-1.0f, -1.0f, -1.0f, 1.0f),
		Vector4<float>(1.0f, -1.0f, -1.0f, 1.0f)
	};

	// and 32 lines
	int line[32][2] = {
		{ 0, 1 },{ 1, 2 },{ 2, 3 },{ 3, 0 },
		{ 4, 5 },{ 5, 6 },{ 6, 7 },{ 7, 4 },
		{ 0, 4 },{ 1, 5 },{ 2, 6 },{ 3, 7 },

		{ 0 + 8, 1 + 8 },{ 1 + 8, 2 + 8 },{ 2 + 8, 3 + 8 },{ 3 + 8, 0 + 8 },
		{ 4 + 8, 5 + 8 },{ 5 + 8, 6 + 8 },{ 6 + 8, 7 + 8 },{ 7 + 8, 4 + 8 },
		{ 0 + 8, 4 + 8 },{ 1 + 8, 5 + 8 },{ 2 + 8, 6 + 8 },{ 3 + 8, 7 + 8 },

		{ 0, 8 },{ 1, 9 },{ 2, 10 },{ 3, 11 },
		{ 4, 12 },{ 5, 13 },{ 6, 14 },{ 7, 15 },
	};

	// map point to 4d space
	Vector4<float> z4 = Vector4<float>(point.x, point.y, point.z, 0.0f);

	Vector4<float> t;
	Vector4<float> tempt;

	// distance to edge
	float distanceEdge = D3D10_FLOAT32_MAX;
	for (int i = 0; i < 32; i++)
	{
		Vector4<float> A = pt[line[i][0]];
		Vector4<float> B = pt[line[i][1]];

		Vector4<float> AP = (z4 - A);
		Vector4<float> lineDir = (B - A).Normalize();
		float APPerpendicular = AP * lineDir;

		float distance;
		if (APPerpendicular > 0.0f && APPerpendicular < (A - B).Length())
		{
			Vector4<float> tempt = A + lineDir * APPerpendicular;
			distance = (tempt - z4).Length();
		}
		else
		{
			float distanceA = (A - z4).Length();
			float distanceB = (B - z4).Length();
			if (distanceA < distanceB)
			{
				distance = distanceA;
				tempt = A;
			}
			else
			{
				distance = distanceB;
				tempt = B;
			}
		}

		if (distance < distanceEdge)
		{
			distanceEdge = distance;
			t = tempt;
		}
	}

	float distanceFinal = (t - z4).Length();

	{
		distanceFinal = distanceFinal - 0.1f;
	}

	if (distanceFinal < 0.0f)
	{
		distanceFinal = 0.0f;
	}
	return distanceFinal;
}

Vector3<float> GetNormal(Vector3<float> point) restrict(amp)
{
	Vector3<float> normal = Vector3<float>(DE(point + Vector3<float>(0.001f, 0, 0)) - DE(point - Vector3<float>(0.001f, 0, 0)),
		DE(point + Vector3<float>(0, 0.001f, 0)) - DE(point - Vector3<float>(0, 0.001f, 0)),
		DE(point + Vector3<float>(0, 0, 0.001f)) - DE(point - Vector3<float>(0, 0, 0.001f))).Normalize();
	return normal;
}