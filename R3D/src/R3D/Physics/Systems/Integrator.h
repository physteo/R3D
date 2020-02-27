#pragma once

#include <R3D/Ecs/System.h>

namespace r3d
{

	class ForceIntegrator : public System
	{
	public:
		virtual void update(ArchetypeManager & am, double t, double dt) override;
	};

	class VelocityIntegrator : public System
	{
	public:
		virtual void update(ArchetypeManager & am, double t, double dt) override;
	};

}