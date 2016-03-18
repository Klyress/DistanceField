#pragma once

// calculate shadow by using shadow ray and approm soft shadow technique from http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
// need normal to avoid immediate escape 
float castShadow(const Vector3<float>& p, const Vector3<float>& light, const float threshold, const Vector3<float>& normal) restrict(amp)
{
	Vector3<float> from = p + normal * threshold;
	Vector3<float> dir = (light - p).Normalize();
	int numIterate = 0;
	float shadowStrength = 1.0f; // pretty confusing, 1.0 means no shadow at all!!
	float distance = D3D10_FLOAT32_MAX;
	float totalDistance = 2.0f * threshold;
	while (numIterate < 50)
	{
		Vector3<float> pShadow = from + dir * totalDistance;
		Vector3<float> dummy;
		Vector3<float> n = normal;
		distance = DE(pShadow);
		totalDistance += distance;
		if (distance < threshold)
		{
			shadowStrength = 0.0f;
			break;
		}

		shadowStrength = min(shadowStrength, 8.0f * distance / totalDistance);
		numIterate++;
	}
	return shadowStrength;
}

Vector3<float> Matching(Ray<float>& ray, const float threshold, const bool isShadowOn) restrict(amp)
{
	Vector3<float> dir = ray.getDirection();
	Vector3<float> p;

	Vector3<float> normal;
	quaternion<float> qnormal;

	quaternion<float> p1;

	float distance = D3D10_FLOAT32_MAX;

	float totalDistance = 0.0f;

	int numIterate = 0;
	int maxIteration = 100;
	bool hit = false;
	Vector3<float> c;
	while (numIterate < maxIteration)
	{
		p = ray.from + dir * totalDistance;
		distance = DE(p);
		totalDistance += distance;

		if (totalDistance > 1000.0f) // too far to hit something
		{
			break;
		}

		if (distance < threshold)
		{
			hit = true;
			break;
		}
		numIterate++;
	}

	// a very cheap AO, idea from syntopia
	float k = 1.0f - (float)numIterate / (float)maxIteration;

	if (hit)
	{
		Vector3<float> normal = GetNormal(p, threshold);

		Vector3<float> color;
		color = GetColor(p);
		// cast shadow ray, using approm soft shadow
		Vector3<float> light(-50.0f, 50.0f, -50.0f);
		float shadowStrength = 1.0f; // 1.0 means no shadow at all
		if (isShadowOn)
		{
			shadowStrength = castShadow(p, light, threshold, normal);
		}

		Vector3<float> lightDir = (light - p).Normalize();
		float intense = clamp(lightDir * normal, 0.0f, 1.0f);
		//Vector3<float> eyeDir = (eye - p).Normalize();
		//float si = clamp((lightDir + eyeDir).Normalize() * normal, 0.0f, 1.0f);
		k = k*0.5f + (intense * 0.5f) * shadowStrength;

		color = color * k;
		color.w = totalDistance;
		return color;
		//color = color + Vector3<float>(s, s, s);
	}
	else
	{
		Vector3<float> color(0.0f, 0.0f, 0.0f);
		color.w = 1.0f;
		return color;
	}

}

