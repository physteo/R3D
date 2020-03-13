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
		std::unordered_map<Entity, size_t, EntityHasher> m_entity_to_archetypeData;
		std::vector<ArchetypeData> m_archetypeDataVector;

		ArchetypeManager()
		{
		}

		const Archetype* getArchetype(Entity entity) const
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				size_t archetypeIndex = found->second;
				auto archetype = &m_archetypeDataVector[archetypeIndex];
				return &archetype->getArchetype();
			}
			else
			{
				return nullptr;
			}
		}

		template <class C>
		bool has(Entity entity)
		{
			// entity is already registered in an archetype?
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				//yes
				size_t archetypeIndex = found->second;
				auto archetype = &m_archetypeDataVector[archetypeIndex];
				// does the archetype of entity have C?
				if (archetype->has<C>())
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
				auto archetype = &m_archetypeDataVector[archetypeIndex];
				// does the archetype of entity have C?
				if (archetype->has<C>())
				{
					// get
					RawPackedArray* addC = archetype->get<C>();
					return (C*)addC->getComponent(entity);
				}
				else
				{
					R3D_CORE_ERROR("[ ECS ] entity {0} does not have component {1}.", entity.id, typeid(C).name());
					R3D_CORE_ASSERT(false, "");
					return nullptr;
				}
			}
			else
			{
				R3D_CORE_ERROR("[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				R3D_CORE_ASSERT(false, "");
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
				auto archetype = &m_archetypeDataVector[archetypeIndex];
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
				R3D_CORE_ERROR("[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				R3D_CORE_ASSERT(false, "");
				return;
			}
		}

		void setArchetype(Entity entity, const Archetype& archetype)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				R3D_CORE_ERROR("[ ECS ] entity {0} already belongs to an archetype.", entity.id);
				R3D_CORE_ASSERT(false, "");
				return;
			}
			else
			{
				ComponentList list = archetype.getComponentList();
				// does an archetype like this exist already?
				auto found_index_pair = matchExact(list);
				if (true == found_index_pair.first)
				{
					// yes: assign entity to that one
					m_entity_to_archetypeData[entity] = found_index_pair.second;
					ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[found_index_pair.second];
					destinationArchetypeData->assignDefault(entity);
				}
				else
				{
					// no: create one and assign entity there
					Archetype wantedArchetype{ list };
					m_archetypeDataVector.emplace_back(wantedArchetype);
					m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
					ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[m_archetypeDataVector.size() - 1];
					destinationArchetypeData->assignDefault(entity);
				}
			}
		}

		template <class ...Args>
		void setArchetype(Entity entity)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				// assert
				R3D_CORE_ERROR("[ ECS ] entity {0} already belongs to an archetype.", entity.id);
				R3D_CORE_ASSERT(false, "");
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

					auto found_index_pair = matchExact(wantedComponentList);
					if (found_index_pair.first)
					{
						// yes: move entity from old archetype to new archetype
						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[found_index_pair.second];
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype::buildMetaType<C>());
						m_entity_to_archetypeData[entity] = found_index_pair.second;
					}
					else
					{
						// no:  create wanted archetype and add entity there
						Archetype wantedArchetype{ wantedComponentList };
						m_archetypeDataVector.emplace_back(wantedArchetype);
						oldArchetype = &m_archetypeDataVector[oldArchetypeIndex];

						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector.back();
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype::buildMetaType<C>());
						m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
					}


				}
				else
				{
					R3D_CORE_ERROR("[ ECS ] entity {0} does not have component {1}.", entity.id, typeid(C).name());
					R3D_CORE_ASSERT(false, "");
					return;
				}
			}
			else
			{
				R3D_CORE_ERROR("[ ECS ] entity {0} is not registered to any Archetype.", entity.id);
				R3D_CORE_ASSERT(false, "");
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
					R3D_CORE_ERROR("[ ECS ] entity {0}  already has component {1}", entity.id, typeid(C).name());
					R3D_CORE_ASSERT(false, "");
					return;
				}
				else
				{
					// no: is there an archetype that has all the old components, plus C?
					ComponentList wantedComponentList = oldArchetype->getComponentList();
					wantedComponentList.getMetatypes().push_back(Metatype::buildMetaType<C>());
					wantedComponentList.finish();

					auto found_index_pair = matchExact(wantedComponentList);
					if (found_index_pair.first)
					{
						// yes: move entity from old archetype to new archetype
						ArchetypeData* destinationArchetypeData = &m_archetypeDataVector[found_index_pair.second];
						migrate(entity, destinationArchetypeData, oldArchetype, Metatype{});
						m_entity_to_archetypeData[entity] = found_index_pair.second;

						// assign the new data
						RawPackedArray* addC = destinationArchetypeData->get<C>();
						addC->assign<C>(entity, std::move(data));

					}
					else
					{
						// no:  create wanted archetype and add entity there
						Archetype wantedArchetype{ wantedComponentList };
						m_archetypeDataVector.emplace_back(wantedArchetype);
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

				auto found_index_pair = matchExact(list);
				if (found_index_pair.first)
				{
					// yes: add entity to the archetype, update map
					m_archetypeDataVector[found_index_pair.second].componentDataMap[typeid(C).hash_code()].assign<C>(entity, std::move(data));
					m_entity_to_archetypeData[entity] = found_index_pair.second;
				}
				else
				{
					// no: create such archetype, update map
					Archetype destinationArchetypeData{ list };
					m_archetypeDataVector.emplace_back(destinationArchetypeData);
					m_archetypeDataVector[m_archetypeDataVector.size() - 1].componentDataMap[typeid(C).hash_code()].assign<C>(entity, std::move(data));
					m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
				}


			}
		}

		std::vector<size_t> matchAtLeastWithout(const ComponentList& included, const ComponentList& excluded) const
		{
			std::vector<size_t> results;
			size_t cnt = 0;
			auto typesA = included.getMetatypes();
			auto typesE = excluded.getMetatypes();
			for (auto it = m_archetypeDataVector.begin(); it != m_archetypeDataVector.end(); ++it, ++cnt)
			{
				auto typesB = it->getComponentList().getMetatypes();
				std::vector<Metatype> intersection;
				std::set_intersection(typesE.begin(), typesE.end(),
					typesB.begin(), typesB.end(), std::back_inserter(intersection));
				if (intersection.empty())
				{
					bool contained = std::includes(typesB.begin(), typesB.end(),
						typesA.begin(), typesA.end());
					if (contained) results.push_back(cnt);
					else continue;
				}
				else continue;
			}

			return results;
		}

		void copy(Entity destinationEntity, Entity sourceEntity)
		{
			auto archIndex1 = m_entity_to_archetypeData[destinationEntity];
			auto archIndex2 = m_entity_to_archetypeData[sourceEntity];

			if (archIndex1 != archIndex2)
			{
				R3D_CORE_ERROR("[ ECS ] entity {0} and {1} have different archetypes.", destinationEntity.id, sourceEntity.id);
				R3D_CORE_ASSERT(false, "");
				return;
			}
			else
			{
				ArchetypeData* source = &m_archetypeDataVector[archIndex2];

				// loop over the list of component from old Archetype
				for (auto jt = source->componentDataMap.begin(); jt != source->componentDataMap.end(); ++jt)
				{
					RawPackedArray& componentArray = jt->second;

					char* componentData = componentArray.getComponent(sourceEntity);
					size_t size_component = componentArray.getMetaType().getSize();

					componentArray.copy(destinationEntity, componentData);
				}
			}
		}

		void removeEntity(Entity entity)
		{
			auto found = m_entity_to_archetypeData.find(entity);
			if (found != m_entity_to_archetypeData.end())
			{
				size_t archetypeDataIndex = found->second;
				m_archetypeDataVector[archetypeDataIndex].remove(entity);
				m_entity_to_archetypeData.erase(entity);
			}
		}

	private:
		std::pair<bool, size_t> matchExact(const ComponentList& list) const
		{
			for (size_t i = 0; i < m_archetypeDataVector.size(); i++)
			{
				if (m_archetypeDataVector[i].getComponentList().getListId() == list.getListId())
				{
					return { true, i };
					break;
				}
			}
			return { false, 0 };
		}

		void migrate(Entity entity, ArchetypeData* destination, ArchetypeData* source, Metatype excluded)
		{
			// loop over the list of component from old Archetype
			for (auto jt = source->componentDataMap.begin(); jt != source->componentDataMap.end(); ++jt)
			{
				auto componentHash = jt->first;
				RawPackedArray& wantedComponentSparseSet = jt->second;
				if (componentHash != excluded.getHash())
				{
					RawPackedArray& newComponentSparseSet = destination->componentDataMap[componentHash];

					char* componentData = wantedComponentSparseSet.getComponent(entity);
					size_t size_component = wantedComponentSparseSet.getMetaType().getSize();

					// move to new
					newComponentSparseSet.assign(entity, componentData);
					// delete from old
				}
				wantedComponentSparseSet.remove(entity);
			}
		}


		bool onEvent(Event& e) override
		{
			EventDispatcher dispatcher(e);
			HANDLE_DISPATCH(dispatcher, EntityDestroyedEvent, ArchetypeManager::onEntityDestroyedEvent);
			HANDLE_DISPATCH(dispatcher, ManyEntitiesDestroyedEvent, ArchetypeManager::onManyEntitiesDestroyedEvent);

			return false;
		}

		bool onEntityDestroyedEvent(EntityDestroyedEvent& e)
		{
			removeEntity(e.getEntity());
			return false;
		}

		bool onManyEntitiesDestroyedEvent(ManyEntitiesDestroyedEvent& e)
		{
			for (auto& entity : e.entities)
			{
				removeEntity(entity);
			}
			return false;
		}
	};
}
