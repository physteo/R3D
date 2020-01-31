#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct Position
	{
		Position(const real3& vector = real3{ 0.0f }) : vec(vector) {}
		real3 vec;
	};

	struct Rotation
	{
		Rotation(const rquat& quat = rquat{ 1.0, 0.0, 0.0, 0.0f }) : quat(quat) {}
		rquat quat;
	};

	struct Scale
	{
		Scale(const real3& vector = real3{ 0.0f }) : vec(vector) {}
		real3 vec;
	};

}
