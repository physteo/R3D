#include <R3Dpch.h>

#include "EntityManager.h"

namespace r3d
{

	Entity EntityManager::create()
	{
		unsigned index;
		unsigned generation;
		if (m_free_indices.size() > ecs::MIN_FREE_IDS)
		{
			// take from the queue
			index = m_free_indices.front();
			m_free_indices.pop();
			generation = m_generation[index];
		}
		else
		{
			//create new
			index = m_generation.size();
			m_generation.push_back(0);
			generation = 0;
			assert(index < (1 << ecs::ENTITY_INDEX_BITS));
		}

		return Entity{ index, generation };
	}

	bool EntityManager::alive(Entity e) const
	{
		return m_generation[e.index()] == e.generation();
	}

	void EntityManager::destroy(Entity e)
	{
		if (alive(e))
		{
			const unsigned index = e.index();
			++m_generation[index];
			m_free_indices.push(index);
		}
	}


	bool EntityManager::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		HANDLE_DISPATCH(dispatcher, EntityDestroyedEvent, EntityManager::onEntityDestroyedEvent);
		HANDLE_DISPATCH(dispatcher, ManyEntitiesDestroyedEvent, EntityManager::onManyEntitiesDestroyedEvent);

		return false;
	}

	bool EntityManager::onEntityDestroyedEvent(EntityDestroyedEvent& e)
	{
		destroy(e.getEntity());
		return false;
	}

	bool EntityManager::onManyEntitiesDestroyedEvent(ManyEntitiesDestroyedEvent& e)
	{
		for (auto& entity : e.entities)
		{
			destroy(entity);
		}
		return false;
	}
	/*
	bool EntityManager::onManyEntitiesDestroyRequest(ManyEntitiesDestroyRequestEvent& e)
	{

		for (auto et = e.entities->begin(); et != e.entities->end(); ++et)
		{
			EntityDestroyedEvent destroyed{ *et };
			destroy(destroyed.getEntity());
		}
		return false;
	}

	bool EntityManager::onEntityDestroyRequest(EntityDestroyRequestEvent& e)
	{
		EntityDestroyedEvent destroyed(e.getEntity());
		destroy(e.getEntity());
		return false;
	}
	*/
}