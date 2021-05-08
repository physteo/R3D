#include <R3Dpch.h>

#include "SolidPrimitivesRenderer.h"

#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>

#include "../Core/Palette.h"
#include "../Core/VertexArray.h"

namespace r3d
{

	struct SolidRendererData
	{
		// Textures/Materials

		// Meshes
		std::unordered_map<MeshID, VertexArray, MeshIDHasher> meshDatabase;

		// Lights
		PointLight pointLight{};
		SpotLight spotLight{};
		SunLight sunLight{};
		bool spotLightOn{ false };
	};

	static SolidRendererData s_data;

	SolidPrimitivesRenderer::SolidPrimitivesRenderer()
	{
		init();
	}
	
	void SolidPrimitivesRenderer::init()
	{
		// load meshes
		s_data.meshDatabase.insert({ MeshID("cube"), Vaos::createCubeVNTUMesh() });
		s_data.meshDatabase.insert({ MeshID("skydome"), Vaos::createSkyDomeMesh(1.0f) });

		s_data.pointLight.eye = float3{ -9.0, 10.0, -10.0 };
		s_data.pointLight.ambient = Palette::getInstance().white;
		s_data.pointLight.diffuse = Palette::getInstance().white;
		s_data.pointLight.specular = Palette::getInstance().white;
		s_data.pointLight.attenuation.constant = 1.0f;
		s_data.pointLight.attenuation.linear = 0.0f;
		s_data.pointLight.attenuation.quadratic = 0.007f;

		s_data.sunLight.eye = float3{ 13.0f, 6.0f, -10.0f };
		s_data.sunLight.center = float3{ 0.0, 0.0, 0.0 };
		s_data.sunLight.ambient = 0.5f * Palette::getInstance().white;
		s_data.sunLight.diffuse = Palette::getInstance().white;
		s_data.sunLight.specular = Palette::getInstance().white;

		s_data.spotLight.cutOff = 0.9f;
		s_data.spotLight.eye = float3{ -10.0, 10.0, 0.0 };
		s_data.spotLight.direction = -s_data.spotLight.eye;
		s_data.spotLight.ambient = 0.5f * Palette::getInstance().white;
		s_data.spotLight.diffuse = Palette::getInstance().white;
		s_data.spotLight.specular = Palette::getInstance().white;
		s_data.spotLight.attenuation.constant = 1.0f;
		s_data.spotLight.attenuation.linear = 0.0f;//0.015f;
		s_data.spotLight.attenuation.quadratic = 0.007f;
		s_data.spotLightOn = true;
	}

	void SolidPrimitivesRenderer::update(ArchetypeManager& am, double t, double dt)
	{
		// pass all textures to the shader.
		m_currentShader->bind();

		// draw planes
		const VertexArray& plane = Vaos::getInstance().plane;
		plane.bind();
		auto archetypes = am.matchAtLeast(ComponentList::buildList<Transform, Color, PlanePrimitive, MaterialComp>(), {});
		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			auto transform = get<Transform>(am, *archIt);
			auto color = get<Color>(am, *archIt);
			auto materials = get<MaterialComp>(am, *archIt);
			size_t numElements = getSize<Transform>(am, *archIt);
			std::vector<Entity>* entities = getEntities(am, *archIt);

			for (size_t j = 0; j < numElements; ++j)
			{
				const float3& n = glm::normalize(float3{ transform[j].orientation.x, transform[j].orientation.y, transform[j].orientation.z });
				const float& off = transform[j].position.x;

				float3 tangent[2];
				compute_basis(n, tangent[0], tangent[1]);
				fquat q = glm::toQuat(glm::transpose(real3x3{ n, tangent[0], tangent[1] }));
				
				auto model = compute_model_matrix((float3)(off * n), q, real(2.0) * transform[j].scale);
				m_currentShader->setUniformMatrix("model", model, false);
				m_currentShader->setUniformMatrix("normalMat", glm::transpose(glm::inverse(model)), false);
				materialsDatabase[materials[j].name].passUniforms(*m_currentShader);
				// m_currentShader->setUniformValue("material.color", float3{ color[j].vec.r, color[j].vec.g, color[j].vec.b });
				// m_currentShader->setUniformValue("material.shininess", 128.0f);
				plane.draw(GL_TRIANGLES);
			}
		}
		plane.unbind();

		archetypes = am.matchAtLeast(ComponentList::buildList<Transform, Color, BoxPrimitive, MaterialComp, MeshID>(), {});
		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			auto transform = get<Transform>(am, *archIt);
			auto color = get<Color>(am, *archIt);
			auto materials = get<MaterialComp>(am, *archIt);
			auto meshes = get<MeshID>(am, *archIt);
			size_t numElements = getSize<Transform>(am, *archIt);

			for (size_t j = 0; j < numElements; j++)
			{
				auto model = compute_model_matrix(transform[j].position, transform[j].orientation, real(2.0) * transform[j].scale);
				m_currentShader->setUniformMatrix("model", model, false);
				m_currentShader->setUniformMatrix("normalMat", glm::transpose(glm::inverse(model)), false);
				materialsDatabase[materials[j].name].passUniforms(*m_currentShader);
				auto mesh = s_data.meshDatabase.find(meshes[j]);
				if (mesh != s_data.meshDatabase.end())
				{
					mesh->second.bind();
					mesh->second.draw(GL_TRIANGLES);
					mesh->second.unbind();
				}
				else
				{
					R3D_CORE_ERROR("Mesh (ID: {0})does not exist.", meshes[j].getID());
				}
			}
		}

		m_currentShader->unbind();
	}

	PointLight& SolidPrimitivesRenderer::getPointLight() { return s_data.pointLight; }
	SunLight& SolidPrimitivesRenderer::getSunLight() { return s_data.sunLight; }
	SpotLight& SolidPrimitivesRenderer::getSpotLight() { return s_data.spotLight; }
	bool SolidPrimitivesRenderer::isSpotLightOn() { return s_data.spotLightOn; }
	void SolidPrimitivesRenderer::switchSpotLight() { s_data.spotLightOn = !s_data.spotLightOn; }

	void SolidPrimitivesRenderer::drawLights(const float3& cameraPosition) const
	{
		drawPointLight(s_data.pointLight.eye, cameraPosition);
		//drawSunLight(sunLight.eye, sunLight.center, cameraPosition);
	}

	void SolidPrimitivesRenderer::drawSunLight(const float3& position, const float3& center, const float3& cameraPosition) const
	{
		float3 distanceFromCameraVec = position - cameraPosition;
		float distanceFromCameraLen = glm::length(distanceFromCameraVec);

		m_sunLightShader->bind();

		float3 lamppos = position;
		float3 lampsca = float3{ 1.0f };
		fquat lamprot = fquat{ 1.0f, 0.0f, 0.0f, 0.0f };
		float4x4 lampmodel = compute_model_matrix(lamppos, lamprot, lampsca);
		m_sunLightShader->setUniformMatrix("model", lampmodel, false);
		float3 direction = center - position;
		m_sunLightShader->setUniformValue("direction", direction.x, direction.y, direction.z);
		m_sunLightShader->setUniformValue("cameraDistance", distanceFromCameraLen);
		m_sunLightShader->setUniformValue("lightColor", s_data.sunLight.diffuse);

		Vaos::getInstance().point.bind();
		Vaos::getInstance().point.draw(GL_POINTS);
		Vaos::getInstance().point.unbind();

		m_sunLightShader->unbind();
	}

	void SolidPrimitivesRenderer::drawPointLight(const float3& position, const float3& cameraPosition) const
	{
		float3 distanceFromCameraVec = position - cameraPosition;
		float distanceFromCameraLen = glm::length(distanceFromCameraVec);

		m_pointLightShader->bind();

		float3 lamppos = position;
		float3 lampsca = float3{ distanceFromCameraLen } / 100.0f;
		fquat lamprot;
		float3 axis = glm::cross({0,0,1}, distanceFromCameraVec);
		lamprot = glm::normalize(fquat{ distanceFromCameraLen + glm::dot({0,0,1}, distanceFromCameraVec), axis.x, axis.y, axis.z });
		float4x4 lampmodel = compute_model_matrix(lamppos, lamprot, lampsca);
		m_pointLightShader->setUniformMatrix("model", lampmodel, false);
		m_pointLightShader->setUniformValue("lightColor", s_data.pointLight.diffuse);

		glDisable(GL_CULL_FACE);
		Vaos::getInstance().quad.bind();
		Vaos::getInstance().quad.draw(GL_TRIANGLES);
		Vaos::getInstance().quad.unbind();
		glEnable(GL_CULL_FACE);

		m_pointLightShader->unbind();
	}


}
