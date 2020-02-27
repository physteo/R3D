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
		auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<Box, Transform>(), {});

		// same archetype
		for (auto arch1 : archetypes)
		{
			auto transform1 = get<Transform>(am, arch1);
			auto boxes1 = get<Box>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Transform>(am, arch1);

			for (size_t i = 0; i < size1; ++i)
			{
				for (size_t j = i + 1; j < size1; ++j)
				{
					CollisionBox box1;
					box1.e = entities1[i];
					box1.position = boxes1[i].offsetPos + transform1[i].position;
					box1.orientation = boxes1[i].offsetRot * transform1[i].orientation;
					box1.halfSize = transform1[i].scale;
					CollisionBox box2;
					box2.e = entities1[j];
					box2.position = boxes1[j].offsetPos + transform1[j].position;
					box2.orientation = boxes1[j].offsetRot * transform1[j].orientation;
					box2.halfSize = transform1[j].scale;

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
			auto transform1 = get<Transform>(am, arch1);
			auto boxes1 = get<Box>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Transform>(am, arch1);

			for (size_t arch2 : archetypes)
			{
				if (arch1 == arch2) continue;
				auto transform2 = get<Transform>(am, arch2);
				auto boxes2 = get<Box>(am, arch2);
				auto& entities2 = getEntities(am, arch2);
				size_t size2 = getSize<Transform>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{

						CollisionBox box1;
						box1.e = entities1[i];
						box1.position = boxes1[i].offsetPos + transform1[i].position;
						box1.orientation = boxes1[i].offsetRot * transform1[i].orientation;
						box1.halfSize = transform1[i].scale;
						CollisionBox box2;
						box2.e = entities2[j];
						box2.position = boxes2[j].offsetPos + transform2[j].position;
						box2.orientation = boxes2[j].offsetRot * transform2[j].orientation;
						box2.halfSize = transform2[j].scale;

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