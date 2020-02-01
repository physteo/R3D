#pragma once

#include <R3D/Core/MathUtils.h>
#include <R3D/Ecs/Entity.h>

namespace r3d
{
	struct Contact
	{
		//<! Contact point in world coordinates
		real3 contactPoint;

		//<! Contact normal from the first to the second object, in world coordinates.
		real3 contactNormal;

		//<! Penetration along the contact normal passing through the contact point.
		real penetration;
		
		//<! Entities involved in contact
		Entity e1, e2;
	};


	struct CollisionData
	{
		CollisionData(size_t maximumContactsIn = 100)
		{
			maximumContacts = maximumContactsIn;
			totalContacts = 0;
			contacts.reserve(maximumContactsIn);
		}

		std::vector<Contact> contacts;

		size_t maximumContacts;
		size_t totalContacts;
	};


}