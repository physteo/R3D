#pragma once

#include "Colliders.h"
#include "Contact.h"

namespace r3d
{

	struct CollisionData;

	class SphereSphere
	{
	public:
		static unsigned int detect(
			const real3& pos1, const real3& pos2,
			const real3& offPos1, const real3& offPos2,
			const rquat& offRot1, const rquat& offRot2,
			const real& rad1, const real& rad2,
			const Entity& e1, const Entity& e2,
			CollisionData* data
		);

	};

	class SpherePlane
	{
	public:
		static unsigned int detect(
			const real3& spherePos, const real3& sphereOffPos, const rquat& sphereOffRot, const real& rad,
			const real3& planeNormal, const real& planeOff,
			const Entity& eSphere, const Entity& ePlane,
			CollisionData* data
		);
	};

	class BoxPlane
	{
	public:
		static unsigned int detect(const CollisionBox& box, const CollisionPlane& plane, Manifold* manifold);
	};

	class BoxBox
	{
	public:
		static unsigned int detect(const CollisionBox& box1, const CollisionBox& box2, Manifold* manifold);

		static unsigned int detectOld(const CollisionBox& box1, const CollisionBox& box2, Contact& contact);

	};

}
