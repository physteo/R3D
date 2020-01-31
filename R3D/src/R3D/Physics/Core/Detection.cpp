#include <R3Dpch.h>
#include "Detection.h"

namespace r3d
{
	// detection routines
	unsigned int DetectSphereSphere::detect(
		const real3& pos1, const real3& pos2,
		const real3& offPos1, const real3& offPos2,
		const rquat& offRot1, const rquat& offRot2,
		const real& rad1, const real& rad2,
		const Entity& e1, const Entity& e2,
		CollisionData* data)
	{
		if (data->totalContacts >= data->maximumContacts) return 0;

		// Find the vector between the objects.
		real3 midline = pos1 - pos2;
		real magnitude2 = glm::dot(midline, midline);

		// See if it is large enough.
		if (magnitude2 <= 0.0f || magnitude2 >= (rad1 + rad2) * (rad1 + rad2))
		{
			return 0;
		}
		real magnitude = glm::sqrt(magnitude2);

		// We manually create the normal, because we have the magnitude to hand.
		real3 normal = midline * (((real)1.0) / magnitude);
		
		
		Contact contact;
		contact.contactNormal = normal;
		contact.contactPoint = pos1 - midline * (real)0.5;
		contact.penetration = (rad1 + rad2 - magnitude);
		// Write the appropriate data.
		contact.e1 = e1;
		contact.e2 = e2;
		//contact->restitution = data->restitution;
		//contact->friction = data->friction;
		data->sphereSphereContacts.push_back(contact);
		++data->totalContacts;

		return 1;
	}

}