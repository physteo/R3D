#pragma once

#include <R3D/Core/Core.h>
#include <functional>
#include <limits>

namespace r3d
{
	namespace ecs
	{
		using Id = unsigned;
		using Index = unsigned;
		using Gen = unsigned;

		const Id ENTITY_INDEX_BITS = 24;
		const Id ENTITY_INDEX_MASK = ((1 << ENTITY_INDEX_BITS) - 1);

		const Id ENTITY_GENERATION_BITS = 9;
		const Id ENTITY_GENERATION_MASK = ((1 << ENTITY_GENERATION_BITS) - 1);

		const Id ENTITY_NULL = std::numeric_limits<Id>::max();
	}

	struct Entity
	{
		Entity() : id(ecs::ENTITY_NULL) {}

		Entity(ecs::Index index, ecs::Gen generation)
		{
			id = (generation << ecs::ENTITY_INDEX_BITS) | (index);
		}

		ecs::Id id;

		ecs::Index index() const
		{
			return (id & ecs::ENTITY_INDEX_MASK);
		}

		ecs::Gen generation() const
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

	struct EntityHasher
	{
		using argument_type = Entity;
		using result_type = size_t;

		result_type operator()(const argument_type& f) const
		{
			return std::hash<size_t>()(f.id);
		}
	};

}

namespace std
{
	//template<>
	//struct hash<r3d::Entity>
	//{
	//	using argument_type = r3d::Entity;
	//	using result_type = size_t;
	//
	//	result_type operator()(const argument_type& f) const
	//	{
	//		return std::hash<size_t>()(f.id);
	//	}
	//};

	//template<>
	//struct hash<std::pair<r3d::Entity, r3d::Entity> >
	//{
	//	std::size_t operator()(const std::pair<r3d::Entity, r3d::Entity>& k) const
	//	{
	//		using std::size_t;
	//		using std::hash;
	//		using std::string;
	//
	//		// Compute individual hash values for first,
	//		// second and third and combine them using XOR
	//		// and bit shifting:
	//		return (k.first.id << 32) + k.second.id;
	//	}
	//};
}