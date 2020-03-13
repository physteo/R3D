#pragma once
#include "Contact.h"

namespace r3d {

	class EntityManager;
	class ArchetypeManager;

	class CollisionData
	{
	public:
		void clearArbiters(EntityManager* em);
		void preStep(ArchetypeManager* am, real dt);
		void applyImpulses(ArchetypeManager* am, int iterations);
	
		std::unordered_map<ArbiterKey, Arbiter> arbiters;

	};

	typedef std::pair<ArbiterKey, Arbiter> ArbPair;
	typedef std::unordered_map<ArbiterKey, Arbiter>::iterator ArbIter;

}
