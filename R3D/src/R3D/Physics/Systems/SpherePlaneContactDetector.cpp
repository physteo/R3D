#include <R3Dpch.h>

#include "SpherePlaneContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>

namespace r3d
{
	void SpherePlaneContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypesSphere = am.matchAtLeast(ComponentList::buildList<ColliderSphere, Transform>(), {});
		auto archetypesPlane = am.matchAtLeast(ComponentList::buildList<ColliderPlane>(), {});

		for (size_t arch1 : archetypesSphere)
		{
			auto transforms1 = get<Transform>(am, arch1);
			auto spheres1 = get<ColliderSphere>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);

			size_t size1 = getSize<ColliderSphere>(am, arch1);

			for (size_t arch2 : archetypesPlane)
			{
				auto planes2 = get<ColliderPlane>(am, arch2);
				std::vector<Entity>* entities2 = getEntities(am, arch2);
				size_t size2 = getSize<ColliderPlane>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{
						//if (SpherePlane::detect(
						//	positions1[i].vec, spheres1[i].offsetPos, spheres1[i].offsetRot, scales1[i].vec.x,
						//	planes2[j].normal, planes2[j].offset,
						//	entities1[i], entities2[j], m_collisionData))
						//{
						//	//R3D_INFO("Collision {0} {1}", entities1[i].id, entities2[j].id);
						//}

					}
				}

			}
		}

	}
}