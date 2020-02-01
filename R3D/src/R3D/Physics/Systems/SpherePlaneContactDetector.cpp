#include <R3Dpch.h>

#include "SpherePlaneContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Components/PrimitivesComponents.h>
#include <R3D/Physics/Core/Detection.h>

namespace r3d
{
	void SpherePlaneContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypesSphere = am.matchAtLeastWithout(ComponentList::buildList<Sphere, Position, Rotation, Scale>(), {});
		auto archetypesPlane = am.matchAtLeastWithout(ComponentList::buildList<Plane>(), {});

		for (size_t arch1 : archetypesSphere)
		{
			auto positions1 = get<Position>(am, arch1);
			auto rotations1 = get<Rotation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto spheres1 = get<Sphere>(am, arch1);
			auto& entities1 = getEntities(am, arch1);
			size_t size1 = getSize<Position>(am, arch1);

			for (size_t arch2 : archetypesPlane)
			{
				auto planes2 = get<Plane>(am, arch2);
				auto& entities2 = getEntities(am, arch2);
				size_t size2 = getSize<Plane>(am, arch2);

				for (size_t i = 0; i < size1; ++i)
				{
					for (size_t j = 0; j < size2; ++j)
					{
						if (DetectSpherePlane::detect(
							positions1[i].vec, spheres1[i].offsetPos, spheres1[i].offsetRot, scales1[i].vec.x,
							planes2[j].normal, planes2[j].offset,
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