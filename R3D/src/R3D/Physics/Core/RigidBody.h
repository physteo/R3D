#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct RigidBody
	{
		RigidBody
		(
			const real3x3& invI = real3x3{ 1.0 },
			const real3& velocity = real3{ 0.0 },
			const real3& angVelocity = real3{ 0.0 },
			const real3& force = real3{ 0.0 },
			const real3& torque = real3{ 0.0 },
			const real& invMass = real{ 1.0 },
			const real& friction = real{ 0.0 }
		) : invI(invI), velocity(velocity), angVelocity(angVelocity), force(force),
			torque(torque), invMass(invMass), friction(friction) {}

		RigidBody
		(
			const real3x3& invI,
			const real& invMass,
			const real& friction,
			const real3& force
		) : invI(invI), velocity{ 0.0 }, angVelocity{ 0.0 }, force{ force },
			torque{ 0.0 }, invMass(invMass), friction(friction) {}

		real3x3 invI;
		real3 velocity;
		real3 angVelocity;
		real3 force;
		real3 torque;
		real invMass;
		real friction;

	};

}