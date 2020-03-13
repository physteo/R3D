#pragma once
#include <R3D/Core/MathUtils.h>
#include <R3D/Ecs/System.h>
#include <R3D/Graphics/Core/VertexArray.h>
#include <R3D/Graphics/Core/Shader.h>

namespace r3d
{

	struct SunLight
	{
		float3 eye;
		float3 center;
		float3 ambient;
		float3 diffuse;
		float3 specular;
	};

	class Shader;

	class SolidPrimitivesRenderer : public System
	{
	public:
		SolidPrimitivesRenderer();
		inline void setShader(Shader* shader) { m_shader = shader; }
		inline void setLampShader(Shader* shader) { m_lampShader = shader; }
		inline void setSunPosition(float3 position) { sun.eye = position; }
		inline float3 getSunPosition() const { return sun.eye; }
		void drawSun(const float3& position, bool center) const;

		virtual void update(ArchetypeManager& am, double t, double dt) override;
		
	private:

		VertexArray m_vao;
		VertexArray m_lampVao;

		Shader* m_shader;
		Shader* m_lampShader;

		SunLight sun;
	};

}