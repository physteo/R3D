#include <R3Dpch.h>
#include "Debugger.h"

#include <R3D/Ecs/EntityManager.h>
#include <R3D/Ecs/ArchetypeManager.h>

#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Physics/Components/BasicPhysicsComponents.h>
#include <R3D/Physics/Components/PrimitivesComponents.h>
#include <R3D/Ecs/TransformComponent.h>

namespace r3d
{


	void Debugger::draw_segment(const real3& position, const float3& vec, const float3& color)
	{
		fquat q{ 1.0, 0.0, 0.0, 0.0 };
		float3 displacement{ 1.0, 0.0, 0.0 };

		float3 N1{ 1.0f,0.0f,0.0f };
		float3 N2 = vec;

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
		else if (glm::dot(vec - float3{ -1,0,0 }, vec - float3{ -1, 0, 0 }) < R3D_EPSILON)
		{
			displacement *= -1;
		}

		float segLength = glm::length(vec);
		float3 rotated_displacement = q * displacement;
		rotated_displacement = segLength * rotated_displacement / glm::length(rotated_displacement);

		Entity entity_s = m_em->create();
		m_am->setArchetype(entity_s, ComponentList::buildList<Segment, Color, Transform, ContactEntityTag, PrimitiveTag>());
		m_am->set<Segment>(entity_s, Segment{});
		m_am->set<Color>(entity_s, Color{ float4{color.r, color.g, color.b, 1.0} });
		m_am->set<Transform>(entity_s, Transform{ (float3)position + rotated_displacement, q, float3{1.0} });
	}

	void Debugger::draw_polygon(const std::vector<real3>& positions)
	{
		for (int i = 0; i < positions.size(); ++i)
		{
			real3 a = positions[i];
			real3 b = positions[(i + 1) % positions.size()] - positions[i];
			draw_segment(a, b, {1,0,0});
		}
	}

	void Debugger::draw_circle(const real3& position, real rad, const real3& color)
	{
		fquat q{ 1.0, 0.0, 0.0, 0.0 };

		Entity entity_c = m_em->create();
		m_am->setArchetype(entity_c, ComponentList::buildList<Circle, Color, Transform, ContactEntityTag, PrimitiveTag>());
		m_am->set<Circle>(entity_c, Circle{});
		m_am->set<Color>(entity_c, Color{ float4{color.r, color.g, color.b, 1.0} });
		m_am->set<Transform>(entity_c, Transform{ position, q, float3{rad} });
	}
}