#pragma once

#include <R3D/Ecs/System.h>

namespace r3d
{

	class Renderer3DPusher : public System
	{
	public:
		Renderer3DPusher();
		virtual void update(ArchetypeManager& am, double t, double dt) override final;

	};

}