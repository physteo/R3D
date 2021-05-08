#pragma once

#include <R3D/Core/Core.h>

#include "Archetype.h"
#include <R3D/Ecs/EntityEvents.h>
#include <R3D/Events/EventsListener.h>

#include <iostream>

namespace r3d
{

	class ArchetypeManager : public EventsListener
	{

	public:
		ArchetypeManager()
		{
		}

		const Archetype* getArchetype(Entity entity) const;

		template <class C>
		bool has(Entity entity)
		{
			// entity is already registered in an archetype?
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				//yes
				size_t archetypeIndex = found->second;
				// does the archetype of entity have C?
				if (m_archetypeDataVector[archetypeIndex].has<C>())
				{
					return true;
				}
			}
			return false;
		}

		template <class C>
		C* get(Entity entity)
		{
			// entity is already registered?
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				// yes
				size_t archetypeIndex = found->second;
				ArchetypeData* archetype = &m_archetypeDataVector[archetypeIndex];
				// does the archetype of entity have C?
				if (archetype->has<C>())
				{
					// get
					RawPackedArray* addC = archetype->get<C>();
					return (C*)addC->getComponent(entity);
				}
				else
				{
					R3D_CORE_ASSERT(false, "[ ECS ] entity {0} does not have component {1}.", entity.id, typeid(C).name());
					return nullptr;
				}
			}
			else
			{
				R3D_CORE_ASSERT(false, "[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				return nullptr;
			}
		}

		template <class C>
		void set(Entity entity, C data)
		{
			// entity is already registered in an archetype?
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				//yes
				size_t archetypeIndex = found->second;
				ArchetypeData* archetype = &m_archetypeDataVector[archetypeIndex];
				// does the archetype of entity have C?
				if (archetype->has<C>())
				{
					// set
					RawPackedArray* addC = archetype->get<C>();
					addC->set<C>(entity, std::move(data));
					return;
				}
			}
			else
			{
				R3D_CORE_ASSERT(false, "[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				return;
			}
		}

		void setArchetype(Entity entity, const Archetype& archetype);

		template <class ...Args>
		void addArchetype()
		{
			ComponentList list = std::move(ComponentList::buildList<Args...>());

			size_t archId = 0;
			if (matchExact(list, archId))
			{
				R3D_CORE_WARN("[ ECS ] archetype already exists.");
				return;
			}
			else
			{
				addArchetype(Archetype(std::move(list)));
			}
		}

		void addArchetype(const Archetype& archetype);

		template <class ...Args>
		void setArchetype(Entity entity)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				R3D_CORE_ASSERT(false, "[ ECS ] entity {0} already belongs to an archetype.", entity.id);
				return;
			}
			else
			{
				Archetype arch{ std::move(ComponentList::buildList<Args...>()) };
				setArchetype(entity, arch);
			}
		}

		template <class C>
		void remove(Entity entity)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				size_t oldArchetypeIndex = found->second;
				auto oldArchetype = &m_archetypeDataVector[oldArchetypeIndex];

				// does that archetype have C?
				if (oldArchetype->has<C>())
				{
					//Metatype metatypeToRemove = ;
					// is there an archetype that has all the same components, minus C?
					ComponentList wantedComponentList = oldArchetype->getComponentList();
					std::vector<Metatype>& metatypes = wantedComponentList.getMetatypes();// .push_back(Metatype::buildMetaType<C>());
					metatypes.erase(std::remove_if(metatypes.begin(), metatypes.end(), [](Metatype& m) { return m == Metatype::buildMetaType<C>(); }), metatypes.end());
					//metatypes.erase(std::remove_if(metatypes.begin(), metatypes.end(), Metatype::operator), metatypes.end());
					wantedComponentList.finish();

					size_t archIndex = 0;
					if (matchExact(wantedComponentList, archIndex))
					{
						// yes: move entity from old archetype to new archetype
						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[archIndex];
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype::buildMetaType<C>());
						m_entity_to_archetypeData[entity] = archIndex;
					}
					else
					{
						// no:  create wanted archetype and add entity there
						m_archetypeDataVector.emplace_back(wantedComponentList);
						oldArchetype = &m_archetypeDataVector[oldArchetypeIndex];

						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector.back();
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype::buildMetaType<C>());
						m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
					}

				}
				else
				{
					R3D_CORE_ASSERT(false, "[ ECS ] entity {0} does not have component {1}.", entity.id, typeid(C).name());
					return;
				}
			}
			else
			{
				R3D_CORE_ASSERT(false, "[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				return;
			}
		}

		template <class C>
		void add(Entity entity, C data)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				// entity has an archetype
				size_t oldArchetypeIndex = found->second;
				auto oldArchetype = &m_archetypeDataVector[oldArchetypeIndex];

				// does that archetype have C?
				if (oldArchetype->has<C>())
				{
					// yes: assert. entity already has that component
					R3D_CORE_ASSERT(false, "[ ECS ] entity {0}  already has component {1}", entity.id, typeid(C).name());
					return;
				}
				else
				{
					// no: is there an archetype that has all the old components, plus C?
					ComponentList wantedComponentList = oldArchetype->getComponentList();
					wantedComponentList.getMetatypes().push_back(Metatype::buildMetaType<C>());
					wantedComponentList.finish();

					size_t archIndex = 0;
					if (matchExact(wantedComponentList, archIndex))
					{
						// yes: move entity from old archetype to new archetype
						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[archIndex];
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype{});
						m_entity_to_archetypeData[entity] = archIndex;

						// assign the new data
						RawPackedArray* addC = destinationArchetypeData->get<C>();
						addC->assign<C>(entity, std::move(data));
					}
					else
					{
						// no:  create wanted archetype and add entity there
						m_archetypeDataVector.emplace_back(wantedComponentList);
						oldArchetype = &m_archetypeDataVector[oldArchetypeIndex];

						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector.back();
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype{});
						m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
						// assign the new data
						RawPackedArray* addC = destinationArchetypeData->get<C>();
						addC->assign<C>(entity, std::move(data));
					}
				}
			}
			else
			{
				// entity has no archetype yet: is there an archetype that has only C?
				ComponentList list = ComponentList::buildList<C>();

				size_t archIndex = 0;
				if (matchExact(list, archIndex))
				{
					// yes: add entity to the archetype, update map
					m_archetypeDataVector[archIndex].componentDataMap[typeid(C).hash_code()].assign<C>(entity, std::move(data));
					m_entity_to_archetypeData[entity] = archIndex;
				}
				else
				{
					// no: create such archetype, update map
					m_archetypeDataVector.emplace_back(list);
					m_archetypeDataVector[m_archetypeDataVector.size() - 1].componentDataMap[typeid(C).hash_code()].assign<C>(entity, std::move(data));
					m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
				}
			}
		}

		std::vector<size_t> matchAtLeast(const ComponentList& included, const ComponentList& excluded) const;
		void copyEntity(Entity destinationEntity, Entity sourceEntity);
		void removeEntity(Entity entity);

		std::vector<Entity>* getEntities(size_t archId)
		{
			if (archId < m_archetypeDataVector.size())
			{
				ArchetypeData* archetypeData = &m_archetypeDataVector[archId];
				if (!archetypeData->componentDataMap.empty())
				{
					return &archetypeData->componentDataMap.begin()->second.getEntities();
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		template<class C>
		C* getComponents(size_t archId)
		{
			return m_archetypeDataVector[archId].get<C>()->getComponents<C>();
		}

		template<class C>
		size_t getNbComponents(size_t archId)
		{
			return m_archetypeDataVector[archId].get<C>()->size();
		}

	private:
		bool matchExact(const ComponentList& list, size_t& indexOut) const;
		void migrate(Entity entity, ArchetypeData* destination, ArchetypeData* source, Metatype excluded);
		bool onEvent(Event& e) override;
		bool onEntityDestroyedEvent(EntityDestroyedEvent& e);
		bool onManyEntitiesDestroyedEvent(ManyEntitiesDestroyedEvent& e);

		std::unordered_map<Entity, size_t, EntityHasher> m_entity_to_archetypeData;
		std::vector<ArchetypeData> m_archetypeDataVector;
	};
}
