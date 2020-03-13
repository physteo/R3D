#pragma once

#include <R3D/Core/Log.h>
#include "Metatype.h"
#include <R3D/Ecs/Entity.h>

namespace r3d
{
	/*
		RawPackedArray is a container of memory-contiguous elements where insertion and deletion is O(1).
		The properties of the elements (size, constructor, destructor, copy and move operator, etc) are
		specified by a Metatype. The maximum amount of elements is hardcoded.
		TODO: make it expandable.
	*/
	class RawPackedArray
	{
	public:
		RawPackedArray();
		RawPackedArray(Metatype meta);
		~RawPackedArray();

		RawPackedArray(const RawPackedArray& other);
		RawPackedArray& operator=(const RawPackedArray& other);

		RawPackedArray(RawPackedArray&& other);
		RawPackedArray& operator=(RawPackedArray&& other);

		friend bool operator==(const RawPackedArray& lhs, const RawPackedArray& rhs);

		template <class C>
		inline C* getComponents() { return (C*)m_data; }
		char*     getComponent(Entity e);
		inline std::vector<Entity>& getEntities() { return m_dataEntities; }

		template <class C>
		void set(Entity e, C data)
		{
			size_t entityIndex = m_map.find(e)->second;
			C* location = (C*)(m_data + entityIndex * m_meta.getSize());
			*location = std::move(data);
		}

		template <class C>
		void assign(Entity e, C data)
		{
			C* location = (C*)(m_data + m_active * m_meta.getSize());
			*location = std::move(data);
			m_dataEntities[m_active] = std::move(e);
			m_map.insert({ e, m_active });
			++m_active;
			if (m_active >= MAX_ELEMENTS)
			{
				R3D_CORE_CRITICAL("[ECS] Error: maximum number of components {0} exceeded.", MAX_ELEMENTS);
				assert(false);
			}
		}

		void assign(Entity e, char* data);
		void copy(Entity e, char* data);
		void remove(Entity e);
		bool has(Entity e);

		inline size_t size() const { return m_active; }
		inline const Metatype& getMetaType() const { return m_meta; }

	private:
		static constexpr size_t MAX_ELEMENTS = 32768;

		std::unordered_map<Entity, size_t, EntityHasher>	m_map;
		std::vector<Entity>	m_dataEntities;
		size_t			    m_active;

		Metatype m_meta;
		char*	 m_data;

		void release();
		void swapData(RawPackedArray& other);

	};

	class TestingRawArray
	{
	public:
		void testAll() const;

		bool testNonPod() const;

	private:
		class NonPodBase
		{
		protected:
			std::vector<int> values;
		public:
			NonPodBase() {
				values.push_back(27); values.push_back(28); values.push_back(29);
			}
		};

		class NonPodDerived : public NonPodBase
		{
		public:
			NonPodDerived(int a = 0, int b = 0, int c = 0)
			{
				values.at(0) = a;
				values.at(1) = b;
				values.at(2) = c;
			}
			int sum_all() { return values.at(0) + values.at(1) + values.at(2); }
		};

	};

}
