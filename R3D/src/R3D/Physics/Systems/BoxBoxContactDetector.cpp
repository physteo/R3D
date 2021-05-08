#include <R3Dpch.h>

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>
#include "BoxBoxContactDetector.h"

namespace r3d
{

	void BoxBoxContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto start = std::chrono::system_clock::now();

		auto archetypes = am.matchAtLeast(ComponentList::buildList<ColliderBox, Transform>(), {});

		// same archetype
		for (auto arch1 : archetypes)
		{
			Transform* transform1 = get<Transform>(am, arch1);
			ColliderBox* boxes1 = get<ColliderBox>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Transform>(am, arch1);

			for (size_t i = 0; i < size1; ++i)
			{
				for (size_t j = i + 1; j < size1; ++j)
				{
					Manifold newManifold;
					int numNewContacts = BoxBox::detect(transform1[i], (*entities1)[i], transform1[j], (*entities1)[j], &newManifold);
					 
					ArbiterKey key{ (*entities1)[i], (*entities1)[j] };
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

		// different archetypes
		for (auto arch1 : archetypes)
		{
			auto transform1 = get<Transform>(am, arch1);
			auto boxes1 = get<ColliderBox>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Transform>(am, arch1);

			for (size_t arch2 : archetypes)
			{
				if (arch1 == arch2) continue;
				auto transform2 = get<Transform>(am, arch2);
				auto boxes2 = get<ColliderBox>(am, arch2);
				std::vector<Entity>* entities2 = getEntities(am, arch2);
				size_t size2 = getSize<Transform>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{

						Manifold newManifold;
						int numNewContacts = BoxBox::detect(transform1[i], (*entities1)[i], transform2[j], (*entities2)[j], &newManifold);

						ArbiterKey key{ (*entities1)[i], (*entities2)[j] };
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

		auto end = std::chrono::system_clock::now();
		measuredTime += static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		++trials;

	}
}