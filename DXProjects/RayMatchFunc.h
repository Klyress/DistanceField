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

Vector3<float> Matching(Ray<float>& ray, const float threshold, const bool isShadowOn, const float gameTime) restrict(amp)
{
	Vector3<float> dir = ray.getDirection();
	Vector3<float> p;

	Vector3<float> normal;
	quaternion<float> qnormal;

	quaternion<float> p1;

	float distance = D3D10_FLOAT32_MAX;

	float totalDistance = 0.0f;

	int numIterate = 0;
	int maxIteration = 500;
	bool hit = false;
	Vector3<float> c;
	while (numIterate < maxIteration)
	{
		p = ray.from + dir * totalDistance;
		p.w = gameTime;
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
		p.w = gameTime;
		Vector3<float> normal = GetNormal(p, totalDistance*totalDistance*0.1f/800.0f);

		Vector3<float> color;
		color = GetColor(p);
		// cast shadow ray, using approm soft shadow
		Vector3<float> light(0.0f, 100.0f, 10000.0f);
		float shadowStrength = 1.0f; // 1.0 means no shadow at all
		if (isShadowOn)
		{
			shadowStrength = castShadow(p, light, threshold, normal);
		}

		Vector3<float> lightDir = (light - p).Normalize();
		//lightDir = Vector3<float>(0.f, 100.f, 100.0f).Normalize();

		//float intense = clamp(lightDir * normal, 0.0f, 1.0f);

		//k = k*0.5f + (intense * 0.5f) * shadowStrength;

		//color = color * k;
		//color.w = totalDistance;

		float k = normal * (-ray.dir);
		float fresnel = 1.0f - max(normal * (-ray.dir), 0.0f);
		fresnel = pow(fresnel, 3.0f) * 0.65f;
		Vector3<float> reflectColor = getSkyColor((ray.dir).Reflect(normal));

		float diffuse = pow(lightDir * normal * 0.4f + 0.6f, 80.0f);

		Vector3<float> refractedColor = Vector3<float>(0.1f, 0.19f, 0.22f) + color * diffuse * 0.12f;

		color = refractedColor +(reflectColor - refractedColor) * fresnel;

		float atten = max(1.0 - totalDistance * totalDistance * 0.001f, 0.0);
		color = color + Vector3<float>(0.8f, 0.9f, 0.6f) * (p.y - 1.0f) * 0.18f * atten;

		float si = pow(max(ray.dir.Reflect(normal) * lightDir, 0.0f), 60.0f) *((60.0f + 8.0f) / (3.1415f * 8.0f));
		//Vector3<float> specularColor(252.0f/255.0f, 70.0f/255.0f, 53.0f/255.0f);
		Vector3<float> specularColor(1.0f, 1.0f, 1.0f);

		color = color + specularColor * si;
		//color = Vector3<float>(fresnel, fresnel, fresnel);
		//color = Vector3<float>(k, k, k);
		return color;
	}
	else
	{
		Vector3<float> color(0.0f, 0.0f, 0.0f);
		color = getSkyColor(ray.dir);
		color.w = 1.0f;
		return color;
	}

}

