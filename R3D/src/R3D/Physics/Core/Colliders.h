#pragma once
#include <R3D/Ecs/Entity.h>
#include <R3D/Core/MathUtils.h>
#include "Topology.h"

namespace r3d
{

	struct ColliderBox
	{
	};

	struct CollisionSphere
	{
		rquat orientation;
		real3 position;
		real  radius;
		Entity e;
	};

	struct CollisionBox
	{
		rquat orientation;
		real3 position;
		real3 halfSize;
		Entity e;

		inline real3 getAxisX() const { return orientation * real3{ 1,0,0 }; }
		inline real3 getAxisY() const { return orientation * real3{ 0,1,0 }; }
		inline real3 getAxisZ() const { return orientation * real3{ 0,0,1 }; }
		const Topology& getTopology() const { return s_BoxTopology; }

		real3 toWorld(const real3& x) const;
		real3 toWorld(real3&& x) const;
		real3 toLocal(const real3& x) const;
		real3 toLocal(real3&& x) const;

		static real3x3 computeInertiaTensor(const real3& halfSize, const real& mass);

	private:
		static const Topology s_BoxTopology;
 	};

	struct CollisionPlane
	{
		real3 normal;
		real offset;
		Entity e;
	};

}