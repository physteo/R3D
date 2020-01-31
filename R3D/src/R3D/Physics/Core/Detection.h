#pragma once
#include <R3D/Core/MathUtils.h>
#include <R3D/Ecs/Entity.h>

#include "Contact.h"

namespace r3d
{

	class DetectSphereSphere
	{
	public:
		static unsigned int detect(
			const real3& pos1, const real3& pos2,
			const real3& offPos1, const real3& offPos2,
			const rquat& offRot1, const rquat& offRot2,
			const real& rad1, const real& rad2,
			const Entity& e1, const Entity& e2,
			CollisionData* data);

	};

}
