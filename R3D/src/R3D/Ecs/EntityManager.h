#pragma once

/* engine */
#include "EntityEvents.h"
#include <R3D/Events/EventsListener.h>

namespace r3d
{

	namespace ecs
	{
		const unsigned MIN_FREE_IDS = 1024;
	}

	class EntityManager : public EventsListener
	{
	public:
		Entity create();
		bool alive(Entity e) const;
		void destroy(Entity e);

		bool onEvent(Event& e) override;

	private:
		std::vector<unsigned> m_generation;
		std::queue<unsigned>  m_free_indices;

		bool onEntityDestroyedEvent(EntityDestroyedEvent& e);
		bool onManyEntitiesDestroyedEvent(ManyEntitiesDestroyedEvent& e);

	};

}