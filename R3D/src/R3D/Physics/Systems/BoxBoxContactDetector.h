#pragma once

#include <R3D/Ecs/System.h>

namespace r3d
{

	class CollisionData;

	class BoxBoxContactDetector : public System
	{
	public:
		BoxBoxContactDetector(CollisionData* collisionData = nullptr) : m_collisionData(collisionData)
		{
			measuredTime = 0;
			trials = 0;
		}

		void setCollisionData(CollisionData* collisionData) { m_collisionData = collisionData; }
		virtual void update(ArchetypeManager& am, double t, double dt) override;

		int measuredTime;
		int trials;

	private:
		CollisionData* m_collisionData;
	};

}