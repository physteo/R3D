#include <R3Dpch.h>
#include "Detection.h"
#include "World.h"

namespace r3d
{

	unsigned int SpherePlane::detect(
		const real3& spherePos, const real3& sphereOffPos, const rquat& sphereOffRot, const real& rad,
		const real3& planeNormal, const real& planeOff,
		const Entity& eSphere, const Entity& ePlane,
		CollisionData* data
	)
	{
		//if (data->totalContacts >= data->maximumContacts) return 0;

		real ballDistance = glm::dot(planeNormal, spherePos) -rad - planeOff;

		if (ballDistance >= 0) return 0;

		Contact contact;
		////contact.normal = planeNormal;
		////contact.penetration = -ballDistance;
		////contact.position = spherePos - planeNormal * (ballDistance + rad);
		////contact.e1 = eSphere;
		////contact.e2 = ePlane;
		//contact->restitution = data->restitution;
		//contact->friction = data->friction;
		//data->contacts.push_back(contact);
		//++data->totalContacts;

		return 1;
	}

}

