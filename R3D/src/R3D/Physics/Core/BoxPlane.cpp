#include <R3Dpch.h>
#include "Detection.h"
#include "Geometry.h"
#include <R3D/Core/Log.h>

namespace r3d
{
	struct Topology;

	unsigned int BoxPlane::detect(const Transform& box, const Entity& boxEnt, const ColliderPlane& plane, const Entity& planeEnt, Manifold* manifold)
	{
		const Topology& top = BoxBox::getTopology();
		
		bool manifoldBuilt = false;
		for (int i = 0; i < top.vertices.size(); ++i)
		{
			const real3& vertexPos = box.position + box.orientation * (box.scale * top.vertices[i].position);
			real distance;

			if (is_point_below_plane(vertexPos, plane, distance))
			{
				if (false == manifoldBuilt)
				{
					manifold->e1 = planeEnt;
					manifold->e2 = boxEnt;
					manifold->normal = plane.normal;
					manifold->numContacts = 0;
					compute_basis(manifold->normal, manifold->tangent[0], manifold->tangent[1]);
					manifoldBuilt = true;
				}

				// Create the contact data.
				Contact* contact = manifold->contacts + manifold->numContacts;
				contact->type = ContactType::BOXPLANE__POINT_FACE;
				contact->fp.first = 0;
				contact->fp.second = i;
				contact->penetration = -distance;
				contact->position = project_point_to_plane(vertexPos, plane);

				++(manifold->numContacts);
			} 
		}

		return (false == manifoldBuilt) ? 0 : manifold->numContacts;
	}

}