#include <R3Dpch.h>
#include "Camera.h"

namespace r3d
{

	float3 Camera::getCameraX() const
	{
		return glm::normalize(glm::cross(m_up, getCameraZ()));  //TODO: sure?
	}

	float3 Camera::getCameraY() const
	{
		return glm::cross(getCameraZ(), getCameraX());
	}

	float3 Camera::getCameraZ() const
	{
		return glm::normalize(m_eye - m_center);
	}

	float4x4 Camera::getViewMatrix() const
	{
		return glm::lookAt(m_eye, m_center, m_up);
	}

}