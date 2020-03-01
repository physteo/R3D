#pragma once
#include <R3D/Core/MathUtils.h>
#include <R3D/Ecs/TransformComponent.h>
#include "Colliders.h"

#define R3D_CONTACT_EPSILON 0.01

namespace r3d
{

	bool  intersect_segment_plane(const real3& a, const real3& b, const ColliderPlane& p, real3 &q);
	bool  is_point_below_plane   (const real3& point, const ColliderPlane& plane, real& distance, real tolerance = R3D_CONTACT_EPSILON);
	real3 project_point_to_plane (const real3& point, const ColliderPlane& plane);
	int get_deepest_point_id     (const Topology& top, const Transform& box, const ColliderPlane& plane, real& distance);
	void  compute_basis          (const real3& normal, real3& tangent, real3& bitangent);
	real  closest_points_edges   (const real3& origin1, const real3& end1, const real3& origin2, const real3& end2,
		                          real3 &c1, real3 &c2);
	//real3 get_support_point      (const CollisionBox& box, const real3& direction, int& pointID, real& projection);
	int   intersect_line_quad(real3 p, real3 q, real3 a, real3 b, real3 c, real3 d, real tolerance, real3 &r);

}