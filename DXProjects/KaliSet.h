#pragma once

float Kaliset(Vector3<float> pos) restrict(amp)
{
	float zoom = 2.0f;
	Vector3<float> offset = Vector3<float>(0.1f, 0.0f, 0.0f);
	Vector3<float> p = pos*zoom + offset;  // Zoom and Offset values set the scale and relative position of the Kaliset fractal 
	int i = 0;
	float ln = 0;
	float lnprev = 0;
	float expsmooth = 0;

	int Iterations = 10;
	for (i = 0; i < Iterations; i++) 
	{
		// Kaliset formula (Scale and Julia are the params):
		p.x = abs(p.x);
		p.y = abs(p.y);
		p.z = abs(p.z);
		p = p / (p * p); // means that the vector is divided by the square of it's length (inversion)
		p = p*1.2f + Vector3<float>(0.5f, 0.0f, 0.1f);
		// Exp. smoothing calculation:
		lnprev = ln;
		ln = p.Length();
		expsmooth += exp(-1.0f / abs(lnprev - ln));
	}
	return expsmooth;
}