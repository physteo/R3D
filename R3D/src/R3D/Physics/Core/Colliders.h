#pragma once
#include <R3D/Ecs/Entity.h>
#include <R3D/Core/MathUtils.h>
#include "Topology.h"

namespace r3d
{

	struct ColliderBox
	{
		static real3x3 computeInertiaTensor(const real3& halfSize, const real& mass);
	};

	struct ColliderSphere
	{
		rquat orientation;
		real3 position;
		real  radius;
		Entity e;
	};

	struct ColliderPlane
	{
		real3 normal;
		real offset;
	};

}