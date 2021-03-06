#include <R3Dpch.h>

#include "SphereSphereContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Core/RigidBody.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>

namespace r3d
{
	void SphereSphereContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeast(ComponentList::buildList<ColliderSphere, Transform>(), {});

		// same archetype
		for (auto arch1 : archetypes)
		{
			auto transforms1 = get<Transform>(am, arch1);
			auto spheres1 = get<ColliderSphere>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);

			size_t size1 = getSize<ColliderSphere>(am, arch1);

			for (size_t i = 0; i < size1; ++i)
			{
				for (size_t j = i + 1; j < size1; ++j)
				{
					//if (SphereSphere::detect(
					//	positions1[i].vec, positions1[j].vec,
					//	spheres1[i].offsetPos, spheres1[j].offsetPos,
					//	spheres1[i].offsetRot, spheres1[j].offsetRot,
					//	scales1[i].vec.x, scales1[j].vec.x,
					//	entities1[i], entities1[j], m_collisionData))
					//{
					//	//R3D_INFO("Collision {0} {1}", entities1[i].id, entities1[j].id);
					//}
				}
			}
		}

		// different archetypes
		for (auto arch1 : archetypes)
		{
			auto transforms1 = get<Transform>(am, arch1);
			auto spheres1 = get<ColliderSphere>(am, arch1);
			std::vector<Entity>* entities1 = getEntities(am, arch1);

			size_t size1 = getSize<ColliderSphere>(am, arch1);

			for (size_t arch2 : archetypes)
			{
				if (arch1 == arch2) continue;

				auto transforms2 = get<Transform>(am, arch2);
				auto spheres2 = get<ColliderSphere>(am, arch2);
				std::vector<Entity>* entities2 = getEntities(am, arch2);

				size_t size2 = getSize<ColliderSphere>(am, arch2);
				
				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{	
						//if (SphereSphere::detect(
						//	positions1[i].vec, positions2[j].vec,
						//	spheres1[i].offsetPos, spheres2[j].offsetPos,
						//	spheres1[i].offsetRot, spheres2[j].offsetRot,
						//	scales1[i].vec.x, scales2[j].vec.x,
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