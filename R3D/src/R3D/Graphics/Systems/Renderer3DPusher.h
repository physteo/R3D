#pragma once

#include <R3D/Ecs/System.h>
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Core/Material.h>

namespace r3d
{

	class Renderer3DPusher : public System
	{
		Shader* m_currentShader;
		std::unordered_map<std::string, Material> materialsDatabase;

	public:
		Renderer3DPusher();
		virtual void update(ArchetypeManager& am, double t, double dt) override final;

	};

}