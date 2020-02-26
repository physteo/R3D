#include <R3Dpch.h>
#include "Detection.h"
#include "Geometry.h"
#include <R3D/Core/Log.h>

namespace r3d
{
	struct Topology;

	unsigned int BoxPlane::detect(const CollisionBox& box, const CollisionPlane& plane, Manifold* manifold)
	{
		const Topology& top = box.getTopology();

		manifold->e1 = plane.e;
		manifold->e2 = box.e;
		manifold->normal = plane.normal;
		compute_basis(manifold->normal, manifold->tangent[0], manifold->tangent[1]);

		int numContacts = 0;
		for (int i = 0; i < top.vertices.size(); ++i)
		{
			const real3& vertexPos = box.toWorld(box.halfSize * top.vertices[i].position);
			real distance;

			if (is_point_below_plane(vertexPos, plane, distance))
			{
				// Create the contact data.
				Contact* contact = manifold->contacts + numContacts;
				contact->type = ContactType::BOXPLANE__POINT_FACE;
				contact->fp.first = 0;
				contact->fp.second = i;
				contact->penetration = -distance;
				contact->position = project_point_to_plane(vertexPos, plane);

				++numContacts;
			} 
		}

		manifold->numContacts = numContacts;
		return numContacts;
	}

}