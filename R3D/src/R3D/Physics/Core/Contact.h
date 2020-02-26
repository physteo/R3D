#pragma once

#include <R3D/Core/MathUtils.h>
#include <R3D/Ecs/Entity.h>

namespace r3d
{

	typedef std::pair<short, short> FeaturePair;

	enum ContactType
	{
		BOXBOX__EDGE_EDGE,
		BOXBOX__POINT_FACE,
		BOXPLANE__POINT_FACE
	};

	struct Contact
	{
		Contact() : impulseNormal(0.0f), impulseTangent{ 0.0, 0.0 }, massNormal(0.0), massTangent{ 0.0 ,0.0 } {}

		real3 position;          //!< Contact point in world coordinates
		real  penetration;       //!< Penetration along the contact normal passing through the contact point.
		real  impulseNormal;     //!< Impulse along the normal direction
		real  impulseTangent[2]; //!< Friction impulse along the tangential directions
		real  massNormal;        //!< Constraint mass along the normal 
		real  massTangent[2];    //!< Constraint mass along the tangents
		real  bias;              //!< Penetation bias (TODO: add restitution)
		ContactType type; //!< Type of contact
		FeaturePair fp;   //!< Features involved
	};

	struct Manifold
	{
		enum { MAX_CONTACTS = 16 };

		Contact contacts[MAX_CONTACTS];
		int numContacts;

		real3 normal;     //!< From e1 to e2
		real3 tangent[2]; //!< Contact tangent vectors. Together with the normal, they form an orthonormal basis.
		Entity e1, e2;    //!< Entities involved
	};

	class ArchetypeManager;

	struct Arbiter
	{
		const bool warmStart{ true };
		const bool positionCorrection{ true };
		const bool accumulateImpulses{ true };

		Arbiter(Entity b1, Entity b2, Manifold manifold, real friction);
		
		void update(Manifold* newManifold);
		void preStep(ArchetypeManager* am, float inv_dt);
		void applyImpulse(ArchetypeManager* am);
		void printInfo() const;

		Manifold manifold;
		Entity body1;
		Entity body2;
		float friction;
	};

	struct ArbiterKey
	{
		ArbiterKey(Entity e1, Entity e2)
		{
			if (e1 < e2)
			{
				ea = e1; eb = e2;
			}
			else
			{
				ea = e2; eb = e1;
			}
		}

		Entity ea;
		Entity eb;
	};

	inline bool operator == (const ArbiterKey& a1, const ArbiterKey& a2)
	{
		if (a1.ea == a2.ea && a1.eb == a2.eb)
			return true;
	
		return false;
	}

}

namespace std
{
	template<>
	struct hash<r3d::ArbiterKey>
	{
		using argument_type = r3d::ArbiterKey;
		using result_type = size_t;

		result_type operator()(const argument_type& f) const
		{
			size_t x = std::hash<size_t>()(f.ea.id);
			size_t y = std::hash<size_t>()(f.eb.id);

			return ((x + y)*(x + y + 1) / 2) + y;
		}
	};
}




