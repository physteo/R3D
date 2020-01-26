#pragma once

#include "RawPackedArray.h"
#include <R3D/Utils/StringHash.h>

namespace r3d
{

	class ComponentList
	{
	public:
		template<class ...C>
		static ComponentList buildList()
		{
			ComponentList list;
			list.m_metatypes = { Metatype::buildMetaType<C>()... };
			list.finish();
			return list;
		}

		void finish()
		{
			// sort the list
			std::sort(m_metatypes.begin(), m_metatypes.end(), [](auto left, auto right)
			{
				return left.getHash() < right.getHash();
			});

			// create ID
			std::string id;
			for (auto& elem : m_metatypes)
			{
				id.append(elem.getName());
				id.append("-");
			}
			m_id = StringHash::computeHash(id);
		}

		inline       std::vector<Metatype>& getMetatypes() { return m_metatypes; }
		inline const std::vector<Metatype>& getMetatypes() const { return m_metatypes; }
		inline unsigned long long           getListId()    const { return m_id; }
		inline bool empty() const { return m_metatypes.empty(); }

	private:
		std::vector<Metatype> m_metatypes;
		unsigned long long m_id;
	};


	class Archetype
	{
	public:
		Archetype() {}
		Archetype(ComponentList list) : m_componentList(list) {}

		template<class C>
		void addComponent()
		{
			if (hasComponent<C>()) return;
			else
			{
				Metatype m = Metatype::buildMetaType<C>();
				m_componentList.m_metatypes.push_back(m);
				m_componentList.finish();
			}
		}

		template<class C>
		void removeComponent()
		{
			if (hasComponent<C>())
			{
				Metatype m = Metatype::buildMetaType<C>();
				// TODO: optimize here. Find is linear. Can do log complexity
				auto found = std::find(m_componentList.m_metatypes.begin(), m_componentList.m_metatypes.end(), m);
				m_componentList.m_metatypes.erase(found);
			}
			else return;
		}

		template<class C>
		bool hasComponent() const
		{
			for (const auto& elem : m_componentList.getMetatypes())
			{
				if (typeid(C).hash_code() == elem.getHash()) return true;
			}
			return false;
		}

		inline const ComponentList& getComponentList() const { return m_componentList; }

		ComponentList m_componentList;

	private:
	};

	class ArchetypeData
	{
	public:
		std::unordered_map < size_t, RawPackedArray > componentDataMap;

		ArchetypeData() : m_archetype{} {}
		ArchetypeData(Archetype arch) : m_archetype(arch)
		{
			for (auto meta : arch.m_componentList.getMetatypes())
			{
				//auto compName = meta.getName();
				//componentDataMap.emplace(compName, RawPackedArray{ meta });
				auto compHash = meta.getHash();
				componentDataMap.emplace(compHash, RawPackedArray{ meta });
			}
		}

		ArchetypeData(ArchetypeData& other) = default;
		ArchetypeData& operator=(ArchetypeData& other) = default;

		ArchetypeData(ArchetypeData&& other) = default;
		ArchetypeData& operator=(ArchetypeData&& other) = default;

		void assignDefault(Entity entity)
		{
			for (auto& string_packedArray_pair : componentDataMap)
			{
				string_packedArray_pair.second.assign(entity, nullptr); // TODO: change to default constructor
				//componentDataMap.emplace(compName, RawPackedArray{ meta });
				//componentDataMap[compName] = std::move(RawPackedArray{ meta });
			}
		}

		template<class C>
		RawPackedArray* get()
		{
			if (m_archetype.hasComponent<C>())
			{
				return &componentDataMap[typeid(C).hash_code()];
			}
			return nullptr;
		}

		template<class C>
		bool has()
		{
			return m_archetype.hasComponent<C>();
		}

		void remove(Entity entity)
		{
			for (auto& string_packedArray_pair : componentDataMap)
			{
				string_packedArray_pair.second.remove(entity);
			}
		}

		const Archetype& getArchetype() const
		{
			return m_archetype;
		}

		const ComponentList& getComponentList() const
		{
			return m_archetype.m_componentList;
		}

	private:
		Archetype m_archetype;

	};

}
