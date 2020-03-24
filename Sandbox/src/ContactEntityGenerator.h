#pragma once
#include <R3D.h>

namespace r3d
{

	class ContactPointEntityRemover : public System
	{
	public:
		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override;
	};

	class ContactPointEntityGenerator : public System
	{
	public:
		ContactPointEntityGenerator();
		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override;
		void setContacts(const CollisionData* collData) { m_collData = collData; }
		void setShader(const Shader* shader) { m_contactShader = shader; }
		void setCamera(const Camera* camera) { m_camera = camera; }

	private:
		void createContactEntity(r3d::ArchetypeManager& am, const Manifold& manifold, int contactIndex);
		void renderContact(const float4x4& model) const;
		//void renderNormal(const float4x4& model) const;

		const CollisionData* m_collData;
		const Shader* m_contactShader;
		const Camera* m_camera;
		Archetype m_archetypeCollisionPoint;
		Archetype m_archetypeCollisionNormal;
	};

}