#include <R3Dpch.h>
#include "Geometry.h"
#include <R3D/Ecs/TransformComponent.h>

namespace r3d
{

	bool intersect_segment_plane(const real3& a, const real3& b, const ColliderPlane& p, real3 &q)
	{
		// Compute the t value for the directed line ab intersecting the plane
		real3 ab = b - a;
		real t = (p.offset - glm::dot(p.normal, a)) / glm::dot(p.normal, ab);
		// If t in [0..1] compute and return intersection point
		if (t >= 0.0f && t <= 1.0f) {
			q = a + t * ab;
			return true;
		}
		// Else no intersection 
		return false;
	}

	bool is_point_below_plane(const real3& point, const ColliderPlane& plane, real& distance, real tolerance)
	{
		distance = glm::dot(point, plane.normal) - plane.offset;
		return distance <= tolerance;
	}

	real3 project_point_to_plane(const real3& point, const ColliderPlane& plane)
	{ 
		real3 p0 = plane.offset * plane.normal;
		real dist = glm::dot((point - p0), plane.normal);
		return point - dist * plane.normal;
	}

	int get_deepest_point_id(const Topology& top, const Transform& box, const ColliderPlane& plane, real& distance)
	{
		real minDistance = std::numeric_limits<real>::infinity();
		int support = -1;
		for (int i = 0; i < top.vertices.size(); ++i)
		{
			real3 point = box.position + box.orientation * (top.vertices[i].position * box.scale);

			real distance;
			bool point_is_below = is_point_below_plane(point, plane, distance);

			if (point_is_below)
			{
				if (distance < minDistance)
				{
					minDistance = distance;
					support = i;
				}
			}

		}
		distance = minDistance;
		return support;
	}

	// From Ericson (Real-Time Collision Detection):
	// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
	// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
	// distance between between S1(s) and S2(t)
	real closest_points_edges(const real3& origin1, const real3& end1, const real3& origin2, const real3& end2, 
		real3 &c1, real3 &c2)
	{
		real s, t;
		real3 d1 = end1 - origin1; // Direction vector of segment S1
		real3 d2 = end2 - origin2; // Direction vector of segment S2
		real3 r = origin1 - origin2;
		real a = glm::dot(d1, d1); // Squared length of segment S1, always nonnegative
		real e = glm::dot(d2, d2); // Squared length of segment S2, always nonnegative
		real f = glm::dot(d2, r);
		// Check if either or both segments degenerate into points
		if (a <= R3D_EPSILON && e <= R3D_EPSILON) {
			// Both segments degenerate into points
			s = t = (real) 0.0;
			c1 = origin1;
			c2 = origin2;

			return glm::dot(c1 - c2, c1 - c2);
		}
		if (a <= R3D_EPSILON) {
			// First segment degenerates into a point
			s = (real) 0.0;
			t = f / e; // s = 0 => t = (b*s + f) / e = f / e
			t = glm::clamp(t, (real) 0.0, (real) 1.0);
		}
		else {
			real c = glm::dot(d1, r);
			if (e <= R3D_EPSILON) {
				// Second segment degenerates into a point
				t = (real) 0.0;
				s = glm::clamp(-c / a, (real) 0.0, (real) 1.0); // t = 0 => s = (b*t - c) / a = -c / a
			}
			else {
				// The general nondegenerate case starts here
				real b = glm::dot(d1, d2);
				real denom = a * e - b * b; // Always nonnegative
				// If segments not parallel, compute closest point on L1 to L2 and
				// clamp to segment S1. Else pick arbitrary s (here 0)
				if (denom != (real) 0.0) {
					s = glm::clamp((b*f - c * e) / denom, (real) 0.0, (real) 1.0);
				}
				else s = (real) 0.0;
				// Compute point on L2 closest to S1(s) using
				// t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
				t = (b*s + f) / e;
				// If t in [0,1] done. Else clamp t, recompute s for the new value
				// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
				// and clamp s to [0, 1]
				if (t < (real) 0.0) {
					t = (real) 0.0;
					s = glm::clamp(-c / a, (real) 0.0, (real) 1.0);
				}
				else if (t > (real) 1.0) {
					t = (real) 1.0;
					s = glm::clamp((b - c) / a, (real) 0.0, (real) 1.0);
				}
			}
		}
		c1 = origin1 + s * d1;
		c2 = origin2 + t * d2;
		return glm::dot(c1 - c2, c1 - c2);
	}

	//real3 get_support_point(const CollisionBox& box, const real3& direction, int& pointID, real& projection)
	//{
	//	const Topology& top = BoxBox::getTopology();
	//	real largestDot = -std::numeric_limits<real>::infinity();
	//	real3 bestPoint;
	//	for (int i = 0; i < top.vertices.size(); ++i)
	//	{
	//		real3 point = box.toWorld(box.halfSize * top.vertices[i].position);
	//		real dot = glm::dot(direction, point);
	//		if (dot > largestDot)
	//		{
	//			largestDot = dot;
	//			bestPoint = point;
	//			pointID = i;
	//			projection = dot;
	//		}
	//	}
	//	return bestPoint;
	//}

	// Given line pq and ccw quadrilateral abcd, return whether the line
	// pierces the triangle. If so, also return the point r of intersection
	int intersect_line_quad(real3 p, real3 q, real3 a, real3 b, real3 c, real3 d, real tolerance, real3 &r)
	{
		tolerance = glm::abs(tolerance);
		real3 pq = q - p;
		real3 pa = a - p;
		real3 pb = b - p;
		real3 pc = c - p;
		// Determine which triangle to test against by testing against diagonal first
		real3 m = glm::cross(pc, pq);
		real v = glm::dot(pa, m); // ScalarTriple(pq, pa, pc);
		if (v >= 0.0f) {
			// Test intersection against triangle abc
			real u = -glm::dot(pb, m); // ScalarTriple(pq, pc, pb);
			if (u < -tolerance) return false;
			real w = glm::dot(pq, glm::cross(pb, pa));
			if (w < -tolerance) return false;
			// Compute r, r = u*a + v*b + w*c, from barycentric coordinates (u, v, w)
			real denom = 1.0f / (u + v + w);
			u *= denom;
			v *= denom;
			w *= denom; // w = 1.0f - u - v;
			r = u * a + v * b + w * c;
		}
		else {
			// Test intersection against triangle dac
			real3 pd = d - p;
			real u = glm::dot(pd, m); // ScalarTriple(pq, pd, pc);
			if (u < -tolerance) return false;
			real w = glm::dot(pq, glm::cross(pa, pd));
			if (w < -tolerance) return false;
			v = -v;
			// Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w)
			real denom = 1.0f / (u + v + w);
			u *= denom;
			v *= denom;
			w *= denom; // w = 1.0f - u - v;
			r = u * a + v * d + w * c;
		}
		return true;
	}

}