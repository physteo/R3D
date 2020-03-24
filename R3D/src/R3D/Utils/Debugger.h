#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{
	class ArchetypeManager;
	class EntityManager;

	class Debugger
	{
	public:
		void setAm(ArchetypeManager* am){ m_am = am;}
		void setEm(EntityManager* em)	{ m_em = em;}
		
		void draw_segment(const real3& position, const float3& vec, const float3& color);
		void draw_polygon(const std::vector<real3>& positions);
		void draw_circle(const real3& position, real rad, const real3& color);

	private:
		ArchetypeManager* m_am;
		EntityManager* m_em;
	};

}