#include <R3Dpch.h>

#include <R3D/Ecs/EntityManager.h>
#include "World.h"

namespace r3d
{

	void CollisionData::clearArbiters(EntityManager* em)
	{
		for (auto arb = arbiters.begin(); arb != arbiters.end(); )
		{
			if (false == em->alive(arb->first.ea) || false == em->alive(arb->first.eb))
			{
				arb = arbiters.erase(arb);
			}
			else
			{
				++arb;
			}
		}
	}

	void CollisionData::preStep(ArchetypeManager* am, real dt)
	{
		for (auto arb = arbiters.begin(); arb != arbiters.end(); ++arb)
		{
			arb->second.preStep(am, 1.0 / dt);
		}

	}

	void CollisionData::applyImpulses(ArchetypeManager* am, int iterations)
	{
		for (int i = 0; i < iterations; ++i)
		{
			for (auto arb = arbiters.begin(); arb != arbiters.end(); ++arb)
			{
				arb->second.applyImpulse(am);
			}
		}
	}
	
}