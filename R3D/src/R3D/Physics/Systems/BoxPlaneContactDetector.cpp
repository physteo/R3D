#include <R3Dpch.h>
#include "BoxPlaneContactDetector.h"

#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Components/PrimitivesComponents.h>
#include <R3D/Physics/Core/Detection.h>
#include <R3D/Physics/Core/World.h>

namespace r3d
{
	void BoxPlaneContactDetector::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypesBox = am.matchAtLeastWithout(ComponentList::buildList<Box, Position, Orientation, Scale>(), {});
		auto archetypesPlane = am.matchAtLeastWithout(ComponentList::buildList<Plane>(), {});

		for (size_t arch1 : archetypesBox)
		{
			auto positions1 = get<Position>(am, arch1);
			auto orientations1 = get<Orientation>(am, arch1);
			auto scales1 = get<Scale>(am, arch1);
			auto boxes1 = get<Box>(am, arch1);
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
						CollisionBox box;
						box.e = entities1[i];
						box.position = boxes1[i].offsetPos + positions1[i].vec;
						box.orientation = boxes1[i].offsetRot * orientations1[i].quat;
						box.halfSize = scales1[i].vec;
						
						CollisionPlane plane;
						plane.e = entities2[j];
						plane.normal = planes2[j].normal;
						plane.offset = planes2[j].offset;

						ArbiterKey key{ plane.e, box.e };

						Manifold newManifold;
						int numNewContacts = BoxPlane::detect(box, plane, &newManifold);

						if (numNewContacts > 0)
						{
							ArbIter iter = m_collisionData->arbiters.find(key);
							if (iter == m_collisionData->arbiters.end())
							{
								Arbiter newArb{ plane.e, box.e, newManifold, 0.3 };  
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