#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct RigidBody
	{
		RigidBody
		(
			const real3x3& invI      = real3x3{ 1.0 },
			const real3& velocity    = real3{ 0.0 },
			const real3& angVelocity = real3{ 0.0 },
			const real3& force   = real3{ 0.0 },
			const real3& torque  = real3{ 0.0 },
			const real& invMass  = real{ 1.0 },
			const real& friction = real{ 0.0 }
		) : invI(invI), velocity(velocity), angVelocity(angVelocity), force(force),
			torque(torque), invMass(invMass), friction(friction) {}

		real3x3 invI;
		real3 velocity;
		real3 angVelocity;
		real3 force;
		real3 torque;
		real invMass;
		real friction;
	};

	struct InvMass
	{
		InvMass(real invMass = 1.0) : invMass(invMass) {}
		real invMass;
	};

	struct InvI
	{
		InvI(real3x3 invI = real3x3{ 0.0 }) : invI(invI) {}
		real3x3 invI;
	};

	struct Velocity
	{
		Velocity(const real3& vec = real3{ 0.0 }) : vec(vec) {}
		real3 vec;
	};

	struct AngVelocity
	{
		AngVelocity(const real3& vec = real3{ 0.0 }) : vec(vec) {}
		real3 vec;
	};

	struct TotalForce
	{
		TotalForce(const real3& vec = real3{ 0.0 }) : vec(vec) {}
		real3 vec;
	};

	struct TotalTorque
	{
		TotalTorque(const real3& vec = real3{ 0.0 }) : vec(vec) {}
		real3 vec;
	};

	struct ContactEntityTag { };

}