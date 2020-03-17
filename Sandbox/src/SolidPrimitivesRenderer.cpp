#include "SolidPrimitivesRenderer.h"
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Core/BasicMatrixOperations.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>

#include <R3D/Physics/Core/Geometry.h>

#include "Palette.h"

/* TODO:

	Lights will be entities in the game.
	Shaders are in a database.
	Initially we pass all lights to the one shader that needs it.
	Later, both the shader and the lights will be entities with mutual reference, so there will be
	one system that serves to pass uniforms

*/
namespace r3d
{

	SolidPrimitivesRenderer::SolidPrimitivesRenderer()
	{
		// build light
		pointLight.eye = float3{ 0.0, 10.0, -10.0 };
		pointLight.ambient =  0.5f * Palette::getInstance().white;
		pointLight.diffuse =  Palette::getInstance().white;
		pointLight.specular = Palette::getInstance().white;
		pointLight.attenuation.constant = 1.0f;
		pointLight.attenuation.linear = 0.0f;
		pointLight.attenuation.quadratic = 0.007f;

		sunLight.eye     = float3{ 7.0f, 15.0f, 13.0f };
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

		// plane vao
		{
			std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
			std::vector<float> vertices = {
							-1, -1, 0,
							+1, -1,	0,
							+1, +1, 0,
							-1, +1,	0,
			};
			std::vector<float> normals = {
							0, 0, 1,
							0, 0, 1,
							0, 0, 1,
							0, 0, 1,
			};
			std::vector<float> tangents = {
							1, 0, 0,
							1, 0, 0,
							1, 0, 0,
							1, 0, 0,
			};
			std::vector<unsigned int> components{ 3, 3, 3 };

			m_planeVao = VertexArray{ {vertices, normals, tangents}, components, indices };
		}

		// cube vao
		{
			std::vector<unsigned int> indices =
			{
				0, 1, 2, 0, 2, 3,       // front
				4, 5, 6, 4, 6, 7,       // right
				8, 9, 10, 8, 10, 11,    // back
				12, 13, 14, 12, 14, 15, // left
				16, 17, 18, 16, 18, 19, // up
				20, 21, 22, 20, 22, 23  // down
			};

			std::vector<float> vertices =
			{
				// front
				-0.5, -0.5, +0.5,  // 0
				+0.5, -0.5, +0.5,	// 1
				+0.5, +0.5, +0.5,	// 2
				-0.5, +0.5, +0.5,	// 3
				// right
				+0.5, -0.5, +0.5,	// 4
				+0.5, -0.5, -0.5,	// 5
				+0.5, +0.5, -0.5,	// 6
				+0.5, +0.5, +0.5,  // 7
				// back
				-0.5, -0.5, -0.5,  // 8
				-0.5, +0.5, -0.5,	// 9
				+0.5, +0.5, -0.5,	// 10
				+0.5, -0.5, -0.5,	// 11
				// left
				-0.5, -0.5, -0.5, // 12
				-0.5, -0.5, +0.5, // 13
				-0.5, +0.5, +0.5, // 14
				-0.5, +0.5, -0.5, // 15
				// up
				-0.5, +0.5, +0.5, // 16
				+0.5, +0.5, +0.5, // 17
				+0.5, +0.5, -0.5, // 18
				-0.5, +0.5, -0.5, // 19
				// down
				-0.5, -0.5, -0.5, // 20
				+0.5, -0.5, -0.5, // 21
				+0.5, -0.5, +0.5, // 22
				-0.5, -0.5, +0.5  // 23
			};

			std::vector<float> normals =
			{
				// front
				0., 0., 1.,
				0., 0., 1.,
				0., 0., 1.,
				0., 0., 1.,
				// right
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				// back
				0., 0., -1.,
				0., 0., -1.,
				0., 0., -1.,
				0., 0., -1.,
				// left
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.,
				// up
				0., 1., 0.,
				0., 1., 0.,
				0., 1., 0.,
				0., 1., 0.,
				// down
				0., -1., 0.,
				0., -1., 0.,
				0., -1., 0.,
				0., -1., 0.
			};

			std::vector<float> tangents =
			{
				// front
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				// right
				0., 0., -1.,
				0., 0., -1.,
				0., 0., -1.,
				0., 0., -1.,
				// back
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.,
				// left
				0., 0., 1.,
				0., 0., 1.,
				0., 0., 1.,
				0., 0., 1.,
				// up
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				1., 0., 0.,
				// down
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.,
				-1., 0., 0.
			};

			std::vector<unsigned int> components{ 3, 3, 3 };
			m_cubeVao = VertexArray{ {vertices, normals, tangents }, components, indices };
		}
		
		m_pointLightVao = VertexArray{ {{-1,-1,0, 1,-1,0, 1,1,0, -1,1,0}, {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}}, {3, 4}, {0, 1, 2, 0, 2, 3} };
		m_sunLightVao = VertexArray{ {{0,0,0}, {1,1,1,1}}, {3, 4}, {0} };
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

		m_cubeVao.bind();
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
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}
		m_cubeVao.unbind();

		m_planeVao.bind();
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
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
		m_planeVao.bind();

		m_shader->unbind();


	}

	void SolidPrimitivesRenderer::drawLights(const float3& cameraPosition) const
	{
		drawPointLight(pointLight.eye, cameraPosition);
		drawSunLight(sunLight.eye, sunLight.center, cameraPosition);
		//drawSun(spotLight.eye, false, cameraPosition);
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

		m_sunLightVao.bind();
		glDisable(GL_CULL_FACE);
		glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
		glEnable(GL_CULL_FACE);
		m_sunLightVao.unbind();

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
		float3 a = glm::cross({0,0,1}, distanceFromCameraVec);
		lamprot = glm::normalize(fquat{ distanceFromCameraLen + glm::dot({0,0,1}, distanceFromCameraVec), a.x, a.y, a.z });

		float4x4 lampmodel = compute_model_matrix(lamppos, lamprot, lampsca);

		m_pointLightShader->setUniformMatrix("model", lampmodel, false);

		m_pointLightVao.bind();
		glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		glEnable(GL_CULL_FACE);
		m_pointLightVao.unbind();

		m_pointLightShader->unbind();
	}

}
