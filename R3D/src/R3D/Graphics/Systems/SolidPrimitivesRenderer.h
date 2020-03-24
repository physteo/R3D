#pragma once
#include <R3D/Ecs/System.h>
#include <R3D/Core/MathUtils.h>

namespace r3d
{

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
		inline float3 getSunPosition() const { return pointLight.eye; }
		inline void setShader(Shader* shader) { m_shader = shader; }
		inline void setSunShader(Shader* shader) { m_sunLightShader = shader; }
		inline void setPointShader(Shader* shader) { m_pointLightShader = shader; }
		inline PointLight& getPointLight() { return pointLight; }
		inline SunLight& getSunLight() { return sunLight; }
		inline SpotLight& getSpotLight() { return spotLight; }
		inline void switchSpotLight() { spotLightOn = !spotLightOn; }
		void drawLights(const float3& cameraPosition) const;
		
		virtual void update(ArchetypeManager& am, double t, double dt) override;

	private:
		Shader* m_shader;
		Shader* m_sunLightShader;
		Shader* m_pointLightShader;

		PointLight pointLight;
		SpotLight spotLight;
		SunLight sunLight;
		bool spotLightOn;

		void drawPointLight(const float3& position, const float3& cameraPosition) const;
		void drawSunLight(const float3& position, const float3& center, const float3& cameraPosition) const;
	};

}