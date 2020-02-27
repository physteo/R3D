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
			const real3& scale = real3{ 1.0f }
		) : position(position), orientation(orientation), scale(scale) {}

		rquat orientation;
		real3 position;
		real3 scale;
	};

}
