#pragma once

#include <R3D/Ecs/System.h>

namespace r3d
{

	class CollisionData;

	class SpherePlaneContactDetector : public System
	{
	public:
		SpherePlaneContactDetector(CollisionData* collisionData = nullptr) : m_collisionData(collisionData) {}

		void setCollisionData(CollisionData* collisionData) { m_collisionData = collisionData; }
		virtual void update(ArchetypeManager& am, double t, double dt) override;

	private:
		CollisionData* m_collisionData;

	};

}