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

		void finish();

		inline std::vector<Metatype>& getMetatypes() { return m_metatypes; }
		inline const std::vector<Metatype>& getMetatypes() const { return m_metatypes; }
		inline unsigned long long getListId() const { return m_id; }
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
			if (hasComponent<C>())
			{
				return;
			}
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
	
	private:
		ComponentList m_componentList;

	};

	class ArchetypeData
	{
	public:
		ArchetypeData();
		ArchetypeData(Archetype arch);

		void assignDefault(Entity entity);
		void remove(Entity entity);
		const Archetype& getArchetype() const;
		const ComponentList& getComponentList() const;

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

		std::unordered_map < size_t, RawPackedArray > componentDataMap;

	private:
		Archetype m_archetype;

	};

}
