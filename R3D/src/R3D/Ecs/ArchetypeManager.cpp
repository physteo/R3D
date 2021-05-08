#include <R3Dpch.h>
#include "ArchetypeManager.h"

namespace r3d
{

	const Archetype* ArchetypeManager::getArchetype(Entity entity) const
	{
		auto found = m_entity_to_archetypeData.find(entity);
		if (found != m_entity_to_archetypeData.end())
		{
			size_t archetypeIndex = found->second;
			const ArchetypeData* archetypeData = &m_archetypeDataVector[archetypeIndex];
			return &archetypeData->getArchetype();
		}
		else
		{
			return nullptr;
		}
	}

	void ArchetypeManager::addArchetype(const Archetype& archetype)
	{
		ComponentList list = archetype.getComponentList();
		m_archetypeDataVector.emplace_back(list);
	}

	void ArchetypeManager::setArchetype(Entity entity, const Archetype& archetype)
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
			size_t archIndex = 0;
			if (matchExact(list, archIndex))
			{
				// yes: assign entity to that one
				m_entity_to_archetypeData[entity] = archIndex;
				m_archetypeDataVector[archIndex].assignDefault(entity);
			}
			else
			{
				// no: create one and assign entity there
				m_archetypeDataVector.emplace_back(list);
				m_entity_to_archetypeData[entity] = m_archetypeDataVector.size() - 1;
				m_archetypeDataVector[m_archetypeDataVector.size() - 1].assignDefault(entity);
			}
		}
	}

	std::vector<size_t> ArchetypeManager::matchAtLeast(const ComponentList& included, const ComponentList& excluded) const
	{
		std::vector<size_t> results;
		size_t cnt = 0;
		auto typesA = included.getMetatypes();
		auto typesE = excluded.getMetatypes();
		for (auto it = m_archetypeDataVector.begin(); it != m_archetypeDataVector.end(); ++it, ++cnt)
		{
			const std::vector<Metatype>& typesB = it->getComponentList().getMetatypes();
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

	void ArchetypeManager::copyEntity(Entity destinationEntity, Entity sourceEntity)
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

	void ArchetypeManager::removeEntity(Entity entity)
	{
		auto found = m_entity_to_archetypeData.find(entity);
		if (found != m_entity_to_archetypeData.end())
		{
			size_t archetypeDataIndex = found->second;
			m_archetypeDataVector[archetypeDataIndex].remove(entity);
			m_entity_to_archetypeData.erase(entity);
		}
	}

	bool ArchetypeManager::matchExact(const ComponentList& list, size_t& index) const
	{
		for (size_t i = 0; i < m_archetypeDataVector.size(); i++)
		{
			if (m_archetypeDataVector[i].getComponentList().getListId() == list.getListId())
			{
				index = i;
				return true;
				break;
			}
		}
		index = 0;
		return false;
	}

	void ArchetypeManager::migrate(Entity entity, ArchetypeData* destination, ArchetypeData* source, Metatype excluded)
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

	bool ArchetypeManager::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		HANDLE_DISPATCH(dispatcher, EntityDestroyedEvent, ArchetypeManager::onEntityDestroyedEvent);
		HANDLE_DISPATCH(dispatcher, ManyEntitiesDestroyedEvent, ArchetypeManager::onManyEntitiesDestroyedEvent);

		return false;
	}

	bool ArchetypeManager::onEntityDestroyedEvent(EntityDestroyedEvent& e)
	{
		removeEntity(e.getEntity());
		return false;
	}

	bool ArchetypeManager::onManyEntitiesDestroyedEvent(ManyEntitiesDestroyedEvent& e)
	{
		for (auto& entity : e.entities)
		{
			removeEntity(entity);
		}
		return false;
	}

}