#pragma once
#include "vector3.h"
using namespace xLib;


float sea_octave(float u, float v, float choppy) restrict(amp)
{
	//float w1 = 1.0f - abs(sin(u));
	//float w2 = 1.0f - abs(sin(v));

	//float wavex = abs(cos(u));
	//float wavey = abs(cos(v));

	//w1 = w1 + (wavex - w1) * w1;
	//w2 = w2 + (wavey - w2) * w2;

	//return pow(1.0f - pow(w1 * w2, 0.65f), 1.0f);
	float w1 = sin(u);
	float w2 = sin(v);
	return w1*w2;
}


float SeaHeightMap(Vector3<float> p) restrict(amp)
{
	float freq = 0.16f; // frequncy of sea wave
	float amp = 1.6f;  // height of sea wave
	float choppy = 4.0f; // choppy of sea wave

	float d, h = 0.0f;
	float u = p.x;
	float v = p.z;
	for (int i = 0; i < 10; i++) 
	{
		d = sea_octave(u*freq, v*freq, choppy);
		h += d * amp; 
		freq *= 1.9f;
		amp *= 0.22f;
		choppy = choppy + (1.0f - choppy) * 0.2f;
	}
	return p.y - h;
}

float SeaDE(Vector3<float> point) restrict(amp)
{
	float height = SeaHeightMap(point);
	return height;
}

Vector3<float> GetNormal(Vector3<float> point) restrict(amp)
{
	Vector3<float> n;
	n.y = SeaHeightMap(point);
	n.x = SeaHeightMap(Vector3<float>(point.x + 0.001f, point.y, point.z)) - n.y;
	n.z = SeaHeightMap(Vector3<float>(point.x, point.y, point.z + 0.001f)) - n.y;
	n.y = 0.001f;
	return n.Normalize();
}