#pragma once
#include "vector3.h"
using namespace xLib;

// simple random func
float rand(float u, float v) restrict(amp)
{
	float h = u * 127.1f + v * 311.7f;
	float k = sin(h) * 43758.5453123f;
	float ik = floorf(k);
	return k - ik;
}

float RandomNoise(float u, float v) restrict(amp)
{
	float iu = floorf(u);
	float iv = floorf(v);
	float fu = u - iu;
	float fv = v - iv;

	float ux = fu * fu * (3.0f - 2.0f * fu);
	float uy = fv * fv * (3.0f - 2.0f * fv);

	float r1x = rand(iu, iv);
	float r1y = rand(iu + 1.0f, iv);
	float r2x = rand(iu, iv + 1.0f);
	float r2y = rand(iu + 1.0f, iv + 1.0f);

	float r1 = r1x + (r1y - r1x) * ux;
	float r2 = r2x + (r2y - r2x) * ux;

	return -1.0f + 2.0f * (r1 + (r2 - r1) * uy);
}

float sea_octave(float u, float v, float choppy) restrict(amp)
{
	float r = RandomNoise(u, v);
	u = u + r;
	v = v + r;
	float w1 = 1.0f - fabs(sin(u));
	float w2 = 1.0f - fabs(sin(v));

	float wavex = fabs(cos(u));
	float wavey = fabs(cos(v));

	w1 = w1 + (wavex - w1) * w1;
	w2 = w2 + (wavey - w2) * w2;

	return pow(1.0f - pow(w1 * w2, 0.65f), 4.0f);
}


float SeaHeightMap(Vector3<float> p) restrict(amp)
{
	float freq = 0.16f; // frequncy of sea wave
	float amp = 1.6f;  // height of sea wave
	float choppy = 4.0f; // choppy of sea wave

	float d, h = 0.0f;
	float u = p.x;
	float v = p.z;

	// this is the combination of 10 different frequency and different direction (important!) waves 
	// reuse of u, v is based on we don't care where we start to sample the wave because all wave functions are cycle.
	for (int i = 0; i < 10; i++) 
	{
		d = sea_octave((u + p.w)*freq, (v + p.w)*freq, choppy);
		h += d * amp; 

		// do 2d rotate
		u = u*1.6f + v*1.2f;
		v = v*1.6f - u*1.2f;

		freq *= 1.9f;
		amp *= 0.22f;

		choppy = choppy + (1.0f - choppy) * 0.2f;
	}
	return p.y - h;
}

float DE(Vector3<float> point) restrict(amp)
{
	float height = SeaHeightMap(point);
	return height;
}

float DE(Vector3<float> point) restrict(cpu)
{
	return fabs(point.y);
}
Vector3<float> GetNormal(Vector3<float> point, float delta) restrict(amp)
{
	Vector3<float> n;
	n.y = SeaHeightMap(point);
	n.x = SeaHeightMap(Vector3<float>(point.x + delta, point.y, point.z, point.w)) - n.y;
	n.z = SeaHeightMap(Vector3<float>(point.x, point.y, point.z + delta, point.w)) - n.y;
	n.y = delta;
	return n.Normalize();
}

Vector3<float> GetColor(Vector3<float> point) restrict(amp)
{
	return Vector3<float>(0.8f, 0.9f, 0.6f);
}

Vector3<float> getSkyColor(Vector3<float> e) restrict(amp)
{
	e.y = max(e.y, 0.0f);
	Vector3<float> ret;
	ret.x = pow(1.0f - e.y, 2.0f);
	ret.y = 1.0f - e.y;
	ret.z = 0.6f + (1.0f - e.y)*0.4f;
	return ret;
}