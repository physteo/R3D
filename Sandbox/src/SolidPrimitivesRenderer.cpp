#include "SolidPrimitivesRenderer.h"
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Core/BasicMatrixOperations.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>

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
		sun.center = float3{ 0.0 };
		sun.eye = float3{ 0.0, 10.0, 10.0 };
		sun.ambient =  Palette::getInstance().white;
		sun.diffuse =  Palette::getInstance().white;
		sun.specular = Palette::getInstance().white;

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

		std::vector<float> colors;
		colors.resize(24 * 4);
		for (int i = 0; i < 24 * 4; i+=4)
		{
			colors[i + 0] = 0.8;
			colors[i + 1] = 0.3;
			colors[i + 2] = 0.3;
			colors[i + 3] = 1.0;
		}

		std::vector<unsigned int> components{3, 3, 3, 4};
		m_vao = VertexArray{ {vertices, normals, tangents, colors}, components, indices };
		m_lampVao = VertexArray{ {{-1,-1,0, 1,-1,0, 1,1,0, -1,1,0}, {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}}, {3, 4}, {0, 1, 2, 0, 2, 3} };
	}
	
	void SolidPrimitivesRenderer::update(ArchetypeManager& am, double t, double dt)
	{

		m_shader->bind();
		// pass lights
		m_shader->setUniformValue("sun.direction", glm::normalize(sun.center - sun.eye) );
		m_shader->setUniformValue("sun.ambient",   sun.ambient);
		m_shader->setUniformValue("sun.diffuse",   sun.diffuse);
		m_shader->setUniformValue("sun.specular",  sun.specular);

		m_vao.bind();

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
				m_shader->setUniformMatrix("normalMat", glm::inverse(glm::transpose(model)), false);
				m_shader->setUniformValue("uniColor", float3{ color[j].vec.x, color[j].vec.y, color[j].vec.z });
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}

		m_vao.unbind();
		m_shader->unbind();

		drawSun(sun.eye, false);

	}

	void SolidPrimitivesRenderer::drawSun(const float3& position, bool center) const
	{
		m_lampShader->bind();
		float3 lamppos = position;
		fquat  lamprot{ 1,0,0,0 };
		float3 lampsca{ 0.1 };
		float4x4 lampmodel = compute_model_matrix(lamppos, lamprot, lampsca);
		if (center)
		{
			m_shader->setUniformMatrix("model", lampmodel, false);
			m_shader->setUniformMatrix("view", glm::identity<float4x4>(), false);
			m_shader->setUniformMatrix("projection", glm::identity<float4x4>(), false);
		}
		m_shader->setUniformMatrix("model", lampmodel, false);

		m_lampVao.bind();
		glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glEnable(GL_CULL_FACE);

		m_lampVao.unbind();
		m_lampShader->unbind();
	}

}
