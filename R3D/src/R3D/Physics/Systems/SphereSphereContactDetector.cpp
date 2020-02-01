#include <R3Dpch.h>

#include "SphereSphereContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Components/PrimitivesComponents.h>
#include <R3D/Physics/Core/Detection.h>

namespace r3d
{
	void SphereSphereContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<Sphere, Position, Rotation, Scale>(), {});

		// same archetype
		for (auto arch1 : archetypes)
		{
			auto positions1 = get<Position>(am, arch1);
			auto rotations1 = get<Rotation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto spheres1 = get<Sphere>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Position>(am, arch1);

			for (size_t i = 0; i < size1; ++i)
			{
				for (size_t j = i + 1; j < size1; ++j)
				{
					if (DetectSphereSphere::detect(
						positions1[i].vec, positions1[j].vec,
						spheres1[i].offsetPos, spheres1[j].offsetPos,
						spheres1[i].offsetRot, spheres1[j].offsetRot,
						scales1[i].vec.x, scales1[j].vec.x,
						entities1[i], entities1[j], m_collisionData))
					{
						//R3D_INFO("Collision {0} {1}", entities1[i].id, entities1[j].id);
					}
				}
			}
		}

		// different archetypes
		for (auto arch1 : archetypes)
		{
			auto positions1 = get<Position>(am, arch1);
			auto rotations1 = get<Rotation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto spheres1 = get<Sphere>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Position>(am, arch1);

			for (size_t arch2 : archetypes)
			{
				if (arch1 == arch2) continue;

				auto positions2 = get<Position>(am, arch2);
				auto rotations2 = get<Rotation>(am, arch2);
				auto scales2 = get<Scale>(am, arch2);
				auto spheres2 = get<Sphere>(am, arch2);
				auto& entities2 = getEntities(am, arch2);
				size_t size2 = getSize<Position>(am, arch2);
				
				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{	
						if (DetectSphereSphere::detect(
							positions1[i].vec, positions2[j].vec,
							spheres1[i].offsetPos, spheres2[j].offsetPos,
							spheres1[i].offsetRot, spheres2[j].offsetRot,
							scales1[i].vec.x, scales2[j].vec.x,
							entities1[i], entities2[j], m_collisionData))
						{
							//R3D_INFO("Collision {0} {1}", entities1[i].id, entities2[j].id);
						}

					}
				}

			}
		}

	}
}