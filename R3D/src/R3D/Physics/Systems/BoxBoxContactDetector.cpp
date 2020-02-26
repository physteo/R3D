#include <R3Dpch.h>

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Components/PrimitivesComponents.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>
#include "BoxBoxContactDetector.h"

namespace r3d
{

	void BoxBoxContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<Box, Position, Orientation, Scale>(), {});

		// same archetype
		for (auto arch1 : archetypes)
		{
			auto positions1 = get<Position>(am, arch1);
			auto orientations1 = get<Orientation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto boxes1 = get<Box>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Position>(am, arch1);

			for (size_t i = 0; i < size1; ++i)
			{
				for (size_t j = i + 1; j < size1; ++j)
				{
					CollisionBox box1;
					box1.e = entities1[i];
					box1.position = boxes1[i].offsetPos + positions1[i].vec;
					box1.orientation = boxes1[i].offsetRot * orientations1[i].quat;
					box1.halfSize = scales1[i].vec;
					CollisionBox box2;
					box2.e = entities1[j];
					box2.position = boxes1[j].offsetPos + positions1[j].vec;
					box2.orientation = boxes1[j].offsetRot * orientations1[j].quat;
					box2.halfSize = scales1[j].vec;

					ArbiterKey key{ box1.e, box2.e };

					Manifold newManifold;
					int numNewContacts = BoxBox::detect(box1, box2, &newManifold);
					 
					if (numNewContacts > 0)
					{
						ArbIter iter = m_collisionData->arbiters.find(key);
						if (iter == m_collisionData->arbiters.end())
						{
							Arbiter newArb{ box1.e, box2.e, newManifold, 0.1 };  // TODO: order of the boxes inside arbiter
							m_collisionData->arbiters.insert(ArbPair(key, newArb));
						}
						else
						{
							iter->second.update(&newManifold);
						}
					}
					else
					{
						m_collisionData->arbiters.erase(key);
					}
				}
			}
		}

		// different archetypes
		for (auto arch1 : archetypes)
		{
			auto positions1 = get<Position>(am, arch1);
			auto orientations1 = get<Orientation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto boxes1 = get<Box>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Position>(am, arch1);

			for (size_t arch2 : archetypes)
			{
				if (arch1 == arch2) continue;

				auto positions2 = get<Position>(am, arch2);
				auto orientations2 = get<Orientation>(am, arch2);
				auto scales2 = get<Scale>(am, arch2);
				auto boxes2 = get<Box>(am, arch2);
				auto& entities2 = getEntities(am, arch2);
				size_t size2 = getSize<Position>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{

						CollisionBox box1;
						box1.e = entities1[i];
						box1.position = boxes1[i].offsetPos + positions1[i].vec;
						box1.orientation = boxes1[i].offsetRot * orientations1[i].quat;
						box1.halfSize = scales1[i].vec;
						CollisionBox box2;
						box2.e = entities2[j];
						box2.position = boxes2[j].offsetPos + positions2[j].vec;
						box2.orientation = boxes2[j].offsetRot * orientations2[j].quat;
						box2.halfSize = scales2[j].vec;

						ArbiterKey key{ box1.e, box2.e };

						Manifold newManifold;
						int numNewContacts = BoxBox::detect(box1, box2, &newManifold);

						if (numNewContacts > 0)
						{
							ArbIter iter = m_collisionData->arbiters.find(key);
							if (iter == m_collisionData->arbiters.end())
							{
								Arbiter newArb{ box1.e, box2.e, newManifold, 0.1 };  // TODO: order of the boxes inside arbiter
								m_collisionData->arbiters.insert(ArbPair(key, newArb));
							}
							else
							{
								iter->second.update(&newManifold);
							}
						}
						else
						{
							m_collisionData->arbiters.erase(key);
						}
					}
				}

			}
		}

	}
}