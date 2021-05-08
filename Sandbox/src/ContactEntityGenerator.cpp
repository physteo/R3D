#include "ContactEntityGenerator.h"

namespace r3d
{

	void ContactPointEntityRemover::update(r3d::ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeast(ComponentList::buildList<ContactEntityTag>(), {});
		for (auto arch : archetypes)
		{
			std::vector<Entity>* entities = getEntities(am, arch);
			size_t numEntities = getSize<ContactEntityTag>(am, arch);
			for (size_t i = 0; i < numEntities; ++i)
			{
				Entity e = (*entities)[i];
				Application::getEntityManager()->destroy(e);
				am.removeEntity(e);
			}
		}
	}

	ContactPointEntityGenerator::ContactPointEntityGenerator()
	{
		m_archetypeCollisionPoint = Archetype{ ComponentList::buildList<CirclePrimitive, Color, Transform, ContactEntityTag>() };
		m_archetypeCollisionNormal = Archetype{ ComponentList::buildList<SegmentPrimitive, Color, Transform, ContactEntityTag>() };
	}

	void ContactPointEntityGenerator::update(r3d::ArchetypeManager& am, double t, double dt)
	{
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		m_contactShader->bind();
		for (auto& arbiter : m_collData->arbiters)
		{
			for (size_t i = 0; i < arbiter.second.manifold.numContacts; ++i)
			{
				const Contact& contact = arbiter.second.manifold.contacts[i];
				createContactEntity(am, arbiter.second.manifold, i);
			}
		}
		m_contactShader->unbind();
		glEnable(GL_CULL_FACE);
		//glEnable(GL_DEPTH_TEST);
	}

	void ContactPointEntityGenerator::createContactEntity(r3d::ArchetypeManager& am, const Manifold& manifold, int contactIndex)
	{
		const real3& contactPositionWorld = manifold.contacts[contactIndex].position;

		fquat q{ 1.0, 0.0, 0.0, 0.0 };

		float3 N1{ 1.0f,0.0f,0.0f };
		float3 N2 = (float3)manifold.normal;

		if (glm::length(glm::cross(N2, N1)) > R3D_EPSILON)
		{
			float3 M = glm::normalize(N1 + N2);
			float3 axis = glm::normalize(glm::cross(M, N2));

			float cos = glm::dot(M, N2);
			float sin = glm::length(glm::cross(M, N2));
			q.w = cos;
			q.x = sin * axis.x;
			q.y = sin * axis.y;
			q.z = sin * axis.z;
			q = glm::normalize(q);
		}

		float segLength = 0.333;
		float3 displacement{ 1.0, 0.0, 0.0 };
		float3 rotated_displacement = q * displacement;
		rotated_displacement = segLength * rotated_displacement / glm::length(rotated_displacement);

		//auto entity_s = Application::getEntityManager()->create();
		//am.setArchetype(entity_s, m_archetypeCollisionNormal);
		//am.set<Color>(entity_s, Color{ float4{1.0, 0.0, 0.0, 1.0} });
		//am.set<Transform>(entity_s, Transform{ (float3)contactPositionWorld + rotated_displacement, q, float3{0.3} });
		//auto entity_c = Application::getEntityManager()->create();
		//am.setArchetype(entity_c, m_archetypeCollisionPoint);
		//am.set<Color>(entity_c, Color{ float4{1.0, 0.0, 0.0, 1.0} });
		//am.set<Transform>(entity_c, Transform{ contactPositionWorld, q, float3{0.1} });

		float3 distanceFromCameraVec = contactPositionWorld - m_camera->getEye();
		float distanceFromCameraLen = glm::length(distanceFromCameraVec);
		fquat rot;
		float3 axis = glm::cross({ 0,0,1 }, distanceFromCameraVec);
		rot = glm::normalize(fquat{ distanceFromCameraLen + glm::dot({0,0,1}, distanceFromCameraVec), axis.x, axis.y, axis.z });

		renderContact(compute_model_matrix(contactPositionWorld, rot, float3{0.2f}));
	}

	void ContactPointEntityGenerator::renderContact(const float4x4& model) const
	{
		m_contactShader->setUniformMatrix("model", model, false);

		Vaos::getInstance().circle.bind();
		Vaos::getInstance().circle.draw(GL_TRIANGLES);
		Vaos::getInstance().circle.unbind();
	}

}