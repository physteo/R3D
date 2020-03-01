#include <R3Dpch.h>

#include "Integrator.h"

#include "../Core/RigidBody.h"
#include <R3D/Ecs/TransformComponent.h>

namespace r3d
{

	void ForceIntegrator::update(ArchetypeManager & am, double t, double dt)
	{
		auto archetypesRigidBody = am.matchAtLeastWithout(ComponentList::buildList<Transform, RigidBody>(), {});
		for (auto arch : archetypesRigidBody)
		{
			auto transform = get<Transform>(am, arch);
			auto rigidbody = get<RigidBody>(am, arch);
			size_t numEntities = getSize<Transform>(am, arch);

			for (size_t i = 0; i < numEntities; ++i)
			{
				real3x3 InvIWorld = change_basis_of_matrix(transform[i].orientation, rigidbody[i].invI);

				rigidbody[i].velocity += real(dt) * rigidbody[i].invMass *  rigidbody[i].force;
				rigidbody[i].angVelocity += real(dt) * InvIWorld * rigidbody[i].torque;
			}
		}
	}

	void VelocityIntegrator::update(ArchetypeManager & am, double t, double dt)
	{
		auto archetypesRigidBody = am.matchAtLeastWithout(ComponentList::buildList<Transform, RigidBody>(), {});
		for (auto arch : archetypesRigidBody)
		{
			auto transform = get<Transform>(am, arch);
			auto rigidbody = get<RigidBody>(am, arch);
			size_t numEntities = getSize<RigidBody>(am, arch);

			for (size_t i = 0; i < numEntities; ++i)
			{
				rquat angVelQuat{ 0.0, rigidbody[i].angVelocity.x,  rigidbody[i].angVelocity.y,  rigidbody[i].angVelocity.z };

				transform[i].position += real(dt) *  rigidbody[i].velocity;
				transform[i].orientation = glm::normalize(transform[i].orientation + real(0.5 * dt) * angVelQuat * transform[i].orientation);
			}
		}
	}

}

