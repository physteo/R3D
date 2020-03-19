#include "SolidPrimitivesRenderer.h"
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Core/BasicMatrixOperations.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>

#include <R3D/Physics/Core/Geometry.h>

#include "Palette.h"

namespace r3d
{

	SolidPrimitivesRenderer::SolidPrimitivesRenderer()
	{
		pointLight.eye = float3{ -9.0, 10.0, -10.0 };
		pointLight.ambient =  Palette::getInstance().white;
		pointLight.diffuse =  Palette::getInstance().orange;
		pointLight.specular = Palette::getInstance().white;
		pointLight.attenuation.constant = 1.0f;
		pointLight.attenuation.linear = 0.0f;
		pointLight.attenuation.quadratic = 0.007f;

		sunLight.eye     = float3{ 13.0f, 6.0f, -10.0f };
		sunLight.center  = float3{ 0.0, 0.0, 0.0 };
		sunLight.ambient =  0.5f * Palette::getInstance().white;
		sunLight.diffuse =  Palette::getInstance().white;
		sunLight.specular = Palette::getInstance().white;

		spotLight.cutOff = 0.9f;
		spotLight.eye = float3{ -10.0, 10.0, 0.0 };
		spotLight.direction = -spotLight.eye;
		spotLight.ambient = 0.5f * Palette::getInstance().white;
		spotLight.diffuse = Palette::getInstance().white;
		spotLight.specular = Palette::getInstance().white;
		spotLight.attenuation.constant = 1.0f;
		spotLight.attenuation.linear = 0.0f;//0.015f;
		spotLight.attenuation.quadratic = 0.007f;
		spotLightOn = true;
	}
	
	void SolidPrimitivesRenderer::update(ArchetypeManager& am, double t, double dt)
	{
		m_shader->bind();

		// pass lights
		m_shader->setUniformValue("pointLight.eye",       pointLight.eye );
		m_shader->setUniformValue("pointLight.ambient",   pointLight.ambient);
		m_shader->setUniformValue("pointLight.diffuse",   pointLight.diffuse);
		m_shader->setUniformValue("pointLight.specular",  pointLight.specular);
		m_shader->setUniformValue("pointLight.attenuation.constant",  pointLight.attenuation.constant);
		m_shader->setUniformValue("pointLight.attenuation.linear",    pointLight.attenuation.linear);
		m_shader->setUniformValue("pointLight.attenuation.quadratic", pointLight.attenuation.quadratic);

		m_shader->setUniformValue("spotLight.eye",      spotLight.eye);
		m_shader->setUniformValue("spotLight.direction",   spotLight.direction);
		m_shader->setUniformValue("spotLight.ambient",  float(spotLightOn) * spotLight.ambient);
		m_shader->setUniformValue("spotLight.diffuse",  float(spotLightOn) * spotLight.diffuse);
		m_shader->setUniformValue("spotLight.specular", float(spotLightOn) * spotLight.specular);
		m_shader->setUniformValue("spotLight.cutOff", spotLight.cutOff);
		m_shader->setUniformValue("spotLight.attenuation.constant", spotLight.attenuation.constant);
		m_shader->setUniformValue("spotLight.attenuation.linear", spotLight.attenuation.linear);
		m_shader->setUniformValue("spotLight.attenuation.quadratic", spotLight.attenuation.quadratic);

		m_shader->setUniformValue("sunLight.eye", sunLight.eye);
		m_shader->setUniformValue("sunLight.center", sunLight.center);
		m_shader->setUniformValue("sunLight.ambient", sunLight.ambient);
		m_shader->setUniformValue("sunLight.diffuse", sunLight.diffuse);
		m_shader->setUniformValue("sunLight.specular", sunLight.specular);

		// draw cubes
		const VertexArray& cube = Vaos::get().cube;
		cube.bind();
		auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<Transform, Color, BoxPrimitive>(), {});
		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			auto transform = get<Transform>(am, *archIt);
			auto color = get<Color>(am, *archIt);
			size_t numElements = getSize<Transform>(am, *archIt);
			for (size_t j = 0; j < numElements; j++)
			{
				auto model = compute_model_matrix(transform[j].position, transform[j].orientation, real(2.0) * transform[j].scale);
				m_shader->setUniformMatrix("model", model, false);
				m_shader->setUniformMatrix("normalMat", glm::transpose(glm::inverse(model)), false);
				m_shader->setUniformValue("material.color", float3{ color[j].vec.r, color[j].vec.g, color[j].vec.b });
				m_shader->setUniformValue("material.shininess", 128.0f);
				cube.draw(GL_TRIANGLES);
			}
		}
		cube.unbind();

		// draw planes
		const VertexArray& plane = Vaos::get().plane;
		plane.bind();
		archetypes = am.matchAtLeastWithout(ComponentList::buildList<Transform, Color, PlanePrimitive>(), {});
		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			auto transform = get<Transform>(am, *archIt);
			auto color = get<Color>(am, *archIt);
			size_t numElements = getSize<Transform>(am, *archIt);
			auto& entities = getEntities(am, *archIt);

			for (size_t j = 0; j < numElements; ++j)
			{
				const float3& n = -glm::normalize(float3{ transform[j].orientation.x, transform[j].orientation.y, transform[j].orientation.z });
				const float& off = transform[j].position.x;

				float3 tangent[2];
				compute_basis(n, tangent[0], tangent[1]);
				fquat q = glm::quat_cast(glm::transpose(real3x3{ n, tangent[0], tangent[1] }));

				auto model = compute_model_matrix((float3)(off * n), q, real(2.0) * transform[j].scale);
				m_shader->setUniformMatrix("model", model, false);
				m_shader->setUniformMatrix("normalMat", glm::transpose(glm::inverse(model)), false);
				m_shader->setUniformValue("material.color", float3{ color[j].vec.r, color[j].vec.g, color[j].vec.b });
				m_shader->setUniformValue("material.shininess", 128.0f);
				plane.draw(GL_TRIANGLES);
			}
		}
		plane.unbind();

		m_shader->unbind();
	}

	void SolidPrimitivesRenderer::drawLights(const float3& cameraPosition) const
	{
		drawPointLight(pointLight.eye, cameraPosition);
		drawSunLight(sunLight.eye, sunLight.center, cameraPosition);
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
		m_sunLightShader->setUniformValue("lightColor", sunLight.diffuse);

		Vaos::get().point.bind();
		Vaos::get().point.draw(GL_POINTS);
		Vaos::get().point.unbind();

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
		m_pointLightShader->setUniformValue("lightColor", pointLight.diffuse);

		glDisable(GL_CULL_FACE);
		Vaos::get().quad.bind();
		Vaos::get().quad.draw(GL_TRIANGLES);
		Vaos::get().quad.unbind();
		glEnable(GL_CULL_FACE);

		m_pointLightShader->unbind();
	}

}
