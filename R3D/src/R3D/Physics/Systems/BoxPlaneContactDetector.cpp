#include <R3Dpch.h>
#include "BoxPlaneContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Core/RigidBody.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>

namespace r3d
{
	void BoxPlaneContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypesBox = am.matchAtLeast(ComponentList::buildList<ColliderBox, Transform>(), {});
		auto archetypesPlane = am.matchAtLeast(ComponentList::buildList<ColliderPlane>(), {});

		for (size_t arch1 : archetypesBox)
		{
			auto transform1 = get<Transform>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);

			size_t size1 = getSize<Transform>(am, arch1);

			for (size_t arch2 : archetypesPlane)
			{
				auto planes2 = get<ColliderPlane>(am, arch2);
				std::vector<Entity>* entities2 = getEntities(am, arch2);
				size_t size2 = getSize<ColliderPlane>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{
						Manifold newManifold;
						int numNewContacts = BoxPlane::detect(transform1[i], (*entities1)[i], planes2[j], (*entities2)[j], &newManifold);

						ArbiterKey key{ (*entities2)[j], (*entities1)[i] };
						if (numNewContacts > 0)
						{
							ArbIter iter = m_collisionData->arbiters.find(key);
							if (iter == m_collisionData->arbiters.end())
							{
								Arbiter newArb{ newManifold.e1, newManifold.e2, newManifold };
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