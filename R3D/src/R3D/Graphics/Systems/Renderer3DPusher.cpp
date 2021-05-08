#include <R3Dpch.h>

#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Graphics/Core/VertexArray.h>

#include "SolidPrimitivesRenderer.h" // TODO: temporary: for MeshID
#include "Renderer3DPusher.h"

namespace r3d
{
	struct Renderer3DPusherData
	{
		// Meshes
		std::unordered_map<MeshID, VertexArray, MeshIDHasher> meshDatabase;
	};

	static Renderer3DPusherData s_data;

	Renderer3DPusher::Renderer3DPusher()
	{
		// load meshes
		s_data.meshDatabase.insert({ MeshID("cube"), Vaos::createCubeVNTUMesh() });
		s_data.meshDatabase.insert({ MeshID("skydome"), Vaos::createSkyDomeMesh(1.0f) });
	}

	void Renderer3DPusher::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeast(ComponentList::buildList<Transform, Color, BoxPrimitive, MaterialComp, MeshID>(), {});
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

	}

}