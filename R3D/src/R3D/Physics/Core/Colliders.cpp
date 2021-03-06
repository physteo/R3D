#include <R3Dpch.h>
#include "Colliders.h"

namespace r3d
{
	//real3 CollisionBox::toWorld(const real3& x) const
	//{
	//	return position + orientation * x;
	//}
	//
	//real3 CollisionBox::toWorld(real3&& x) const
	//{
	//	return position + orientation * x;
	//}
	//
	//real3 CollisionBox::toLocal(const real3& x) const
	//{
	//	rquat invOrientation = orientation;
	//	invOrientation.w *= -1;
	//	return invOrientation * (x - position);
	//}
	//
	//real3 CollisionBox::toLocal(real3&& x) const
	//{
	//	rquat invOrientation = orientation;
	//	invOrientation.w *= -1;
	//	return invOrientation * (x - position);
	//}


	real3x3 ColliderBox::computeInertiaTensor(const real3& halfSize, const real& mass)
	{
		real3x3 I{0.0};
		I[0][0] = mass * (halfSize.y * halfSize.y + halfSize.z * halfSize.z) / static_cast<real>(3.);
		I[1][1] = mass * (halfSize.x * halfSize.x + halfSize.z * halfSize.z) / static_cast<real>(3.);
		I[2][2] = mass * (halfSize.y * halfSize.y + halfSize.x * halfSize.x) / static_cast<real>(3.);
		return I;
	}

}