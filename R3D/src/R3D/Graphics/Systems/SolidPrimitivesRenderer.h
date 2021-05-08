#pragma once
#include <R3D/Ecs/System.h>
#include <R3D/Core/MathUtils.h>
#include "../Core/VertexArray.h"
#include "../Core/Material.h"

namespace r3d
{

	struct MeshIDHasher;

	class MeshID
	{
		friend struct MeshIDHasher;

	public:
		MeshID()
		{
			m_id = StringHash::computeHash("");
		}

		MeshID(const std::string& name)
		{
			m_id = StringHash::computeHash(name);
		}

		inline unsigned long long getID() const { return m_id; }

		bool operator==(const MeshID &other) const
		{
			return (m_id == other.m_id);
		}

		bool operator<(const MeshID &other) const
		{
			return (m_id < other.m_id);
		}

	private:
		unsigned long long m_id;
	};

	struct MeshIDHasher
	{
		using argument_type = MeshID;
		using result_type = unsigned long long;

		result_type operator()(const argument_type& f) const
		{
			return std::hash<unsigned long long>()(f.m_id);
		}
	};

	struct LightAttenuation
	{
		float constant;
		float linear;
		float quadratic;
	};

	struct PointLight
	{
		float3 eye;
		float3 ambient;
		float3 diffuse;
		float3 specular;
		LightAttenuation attenuation;
	};

	struct SunLight
	{
		float3 center;
		float3 eye;
		float3 ambient;
		float3 diffuse;
		float3 specular;
	};

	struct SpotLight
	{
		float cutOff;
		float3 eye;
		float3 direction;
		float3 ambient;
		float3 diffuse;
		float3 specular;
		LightAttenuation attenuation;
	};

	class Shader;

	class SolidPrimitivesRenderer : public System
	{
	public:
		SolidPrimitivesRenderer();
		
		void init();
		virtual void update(ArchetypeManager& am, double t, double dt) override;
		void drawLights(const float3& cameraPosition) const;

		inline void setShader(Shader* shader) { m_currentShader = shader; }
		inline void setSunShader(Shader* shader) { m_sunLightShader = shader; }
		inline void setPointShader(Shader* shader) { m_pointLightShader = shader; }
		PointLight& getPointLight();
		SunLight& getSunLight();
		SpotLight& getSpotLight();
		bool isSpotLightOn();
		void switchSpotLight();
		
		std::unordered_map<std::string, Material> materialsDatabase;

	private:
		void drawPointLight(const float3& position, const float3& cameraPosition) const;
		void drawSunLight(const float3& position, const float3& center, const float3& cameraPosition) const;

		Shader* m_currentShader;
		Shader* m_sunLightShader;
		Shader* m_pointLightShader;
	};

}