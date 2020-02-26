#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct Transform
	{
		Transform
		(
			const real3& position = real3{ 0.0f },
			const rquat& orientation = rquat{ 1.0, 0.0, 0.0, 0.0f },
			const real3& scale = real3{ 0.0f }
		) : position(position), orientation(orientation), scale(scale) {}

		rquat orientation;
		real3 position;
		real3 scale;
	};

	struct Position
	{
		Position(const real3& vector = real3{ 0.0f }) : vec(vector) {}
		real3 vec;
	};

	struct Orientation
	{
		Orientation(const rquat& quat = rquat{ 1.0, 0.0, 0.0, 0.0f }) : quat(quat) {}
		rquat quat;
	};

	struct Scale
	{
		Scale(const real3& vector = real3{ 0.0f }) : vec(vector) {}
		real3 vec;
	};

}
