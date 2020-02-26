#pragma once
#include "Contact.h"

namespace r3d {

	struct CollisionData
	{
		CollisionData(size_t maximumContactsIn = 100, real toleranceIn = 0.01)
		{
			maximumContacts = maximumContactsIn;
			totalContacts = 0;
			contacts.reserve(maximumContactsIn);
			tolerance = toleranceIn;
		}

		std::unordered_map<ArbiterKey, Arbiter> arbiters;
		std::vector<Contact> contacts;

		size_t maximumContacts;
		size_t totalContacts;
		float tolerance;
	};

	typedef std::pair<ArbiterKey, Arbiter> ArbPair;
	typedef std::unordered_map<ArbiterKey, Arbiter>::iterator ArbIter;

}
