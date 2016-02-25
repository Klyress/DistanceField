#pragma once
#include "vector3.h"
#include "quaternion.h"
#include <cmath>
using namespace xLib;
template<typename T>
class RTCamera
{
public:
	RTCamera(Vector3<T> eye, Vector3<T> lookAt, Vector3<T> up, float fov, float viewportWidth, float viewportHeight)
	{
		T verticalFov = fov * viewportHeight / viewportWidth;

		m_lookAt = lookAt;
		m_dir = (lookAt - eye).Normalize();

		Vector3<T> centerRay = lookAt - eye;

		T rayLength = centerRay.Length();
		m_left = up.Cross(centerRay).Normalize();
		T lengthx = tan(fov / 2.0f) * rayLength;
		T lengthy = tan(verticalFov / 2) * rayLength;
		Vector3<T> leftVector = m_left * lengthx;
		Vector3<T> upVector = up * lengthy;
		Vector3<T> upperLeftRay = centerRay + leftVector + upVector;
		m_xStep = leftVector * (2.0f / viewportWidth);
		m_yStep = upVector * (2.0f / viewportHeight);
		m_leftTopPoint = eye + upperLeftRay;
		m_eye = eye;
		m_fov = fov;

		m_up = m_dir.Cross(m_left);

		m_viewportWidth = viewportWidth;
		m_viewportHeight = viewportHeight;

		m_sensitiveFactor = 5.0f;
	};

	void RotateCamera(float yaw, float pitch)
	{
		if (fabs(yaw) > 100.0f || fabs(pitch) > 100.0f)
			return;

		yaw /= m_sensitiveFactor;
		pitch /= m_sensitiveFactor;

		// Do yaw first, axis m_up, take yaw as degree
		float yawR = yaw / 180.0f * 3.1415926535897932f;
		float pitchR = pitch / 180.0f * 3.1415926535897932f;
		quaternion<float> qYaw = quaternion<float>(m_up, yawR);
		// rotate m_lookAt
		m_dir = qYaw.Rotate(m_dir);
		m_lookAt = m_eye + m_dir * 1000.0f;
		UpdateCamera();

		quaternion<float> qPitch = quaternion<float>(m_left, pitchR);
		// rotate m_lookAt
		m_dir = qPitch.Rotate(m_dir);
		m_up = qPitch.Rotate(m_up);
		m_lookAt = m_eye + m_dir * 1000.0f;
		UpdateCamera();
	}

	void ZoomCamera(float speedFactor = 1.0f)
	{
		float speed = m_objDistance * speedFactor * 0.2f;
		if (speed > 0.1f)
		{
			speed = 0.1f;
		}
		m_eye = m_eye + m_dir * speed;
		UpdateCamera();
	}

	float GetResolutionFactor()
	{
		return tan(m_fov / m_viewportHeight);
	}

	void UpdateCamera()
	{
		m_dir = (m_lookAt - m_eye).Normalize();

		Vector3<T> centerRay = m_lookAt - m_eye;

		T rayLength = centerRay.Length();
		m_left = m_up.Cross(centerRay).Normalize();
		m_up = m_dir.Cross(m_left);

		T verticalFov = m_fov * m_viewportHeight / m_viewportWidth;
		T lengthx = tan(m_fov / 2.0f) * rayLength;
		T lengthy = tan(verticalFov / 2) * rayLength;
		Vector3<T> leftVector = m_left * lengthx;
		Vector3<T> upVector = m_up * lengthy;
		Vector3<T> upperLeftRay = centerRay + leftVector + upVector;
		m_xStep = leftVector * (2.0f / m_viewportWidth);
		m_yStep = upVector * (2.0f / m_viewportHeight);
		m_leftTopPoint = m_eye + upperLeftRay;
	}
	Vector3<T> m_left;
	Vector3<T> m_up;
	Vector3<T> m_eye;
	Vector3<T> m_lookAt;
	Vector3<T> m_dir;
	Vector3<T> m_xStep, m_yStep;
	Vector3<T> m_leftTopPoint;
	T m_viewportWidth;
	T m_viewportHeight;
	T m_fov;
	T m_objDistance;
	T m_sensitiveFactor;
};

template<typename T>
class TwoDCamera
{
public:
	TwoDCamera(T x, T y, T width, T height, int viewportWidth, int viewportHeight)
	{
		m_viewportWidth = viewportWidth;
		m_viewportHeight = viewportHeight;
		m_width = width;
		m_height = height;
		m_startX = x;
		m_startY = y;
		m_stepX = width / viewportWidth;
		m_stepY = height / viewportHeight;
	}

	void ZoomIn(int x, int y, float scale)
	{
		T posX = m_startX + ((T)x / m_viewportWidth) * m_width;
		T posY = m_startY + ((T)y / m_viewportHeight) * m_height;
		m_width = m_width / scale;
		m_height = m_height / scale;
		m_startX = posX - m_width / 2.0;
		m_startY = posY - m_height / 2.0;
		m_stepX = m_width / m_viewportWidth;
		m_stepY = m_height / m_viewportHeight;
	}

	void ZoomOut(float scale)
	{
		m_width = m_width * scale;
		m_height = m_height * scale;
		m_startX = m_startX - m_width / 2.0;
		m_startY = m_startY - m_height / 2.0;
		m_stepX = m_width / m_viewportWidth;
		m_stepY = m_height / m_viewportHeight;
	}


	T m_startX;
	T m_startY;
	T m_width;
	T m_height;
	T m_stepX;
	T m_stepY;
	int m_viewportWidth;
	int m_viewportHeight;
};