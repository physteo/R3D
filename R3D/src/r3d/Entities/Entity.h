#pragma once

#include <R3D/Core.h>

namespace r3d
{
	namespace ecs
	{
		const unsigned ENTITY_INDEX_BITS = 24;
		const unsigned ENTITY_INDEX_MASK = ((1 << ENTITY_INDEX_BITS) - 1);

		const unsigned ENTITY_GENERATION_BITS = 9;
		const unsigned ENTITY_GENERATION_MASK = ((1 << ENTITY_GENERATION_BITS) - 1);
	}

	struct R3D_API Entity
	{
		Entity() : id(29790) {}

		Entity(unsigned index, unsigned generation)
		{
			id = (generation << ecs::ENTITY_INDEX_BITS) | (index);
		}

		unsigned id;

		unsigned index() const
		{
			return (id & ecs::ENTITY_INDEX_MASK);
		}

		unsigned generation() const
		{
			return ((id >> ecs::ENTITY_INDEX_BITS) & ecs::ENTITY_GENERATION_MASK);
		}

		bool operator==(const Entity &other) const
		{
			return (id == other.id);
		}

		bool operator<(const Entity &other) const
		{
			return (id < other.id);
		}

	};

}

namespace std
{
	template<>
	struct hash<r3d::Entity>
	{
		using argument_type = r3d::Entity;
		using result_type = size_t;

		result_type operator()(const argument_type& f) const
		{
			return std::hash<size_t>()(f.id);
		}
	};

	template<>
	struct hash<std::pair<r3d::Entity, r3d::Entity> >
	{
		std::size_t operator()(const std::pair<r3d::Entity, r3d::Entity>& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:
			return (k.first.id << 32) + k.second.id;
		}
	};
}