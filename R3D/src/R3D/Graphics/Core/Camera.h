#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	class Camera
	{
	private:
		float3 m_eye{1.0f};
		float3 m_center{0.0f};
		float3 m_up{0.0f, 1.0f, 0.0f};

	public:

		Camera() {}
		Camera(const float3& eye, const float3& center, const float3& up) : m_eye(eye), m_center(center), m_up(up) {}

		void setUp(const float3& up) { m_up = up; };
		void setEye(const float3& eye) { m_eye = eye; }
		void setCenter(const float3& center) { m_center = center; }

		const float3& getUp() const { return m_up; }
		const float3& getEye() const { return m_eye; }
		const float3& getCenter() const { return m_center; }

		float3& getUp() { return m_up; }
		float3& getEye() { return m_eye; }
		float3& getCenter() { return m_center; }

		float3 getCameraX() const;
		float3 getCameraY() const;
		float3 getCameraZ() const;

		float4x4 getViewMatrix() const;

	};

}


