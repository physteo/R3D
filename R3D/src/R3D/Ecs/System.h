#pragma once

#include <R3D/Ecs/ArchetypeManager.h>

namespace r3d
{

	class System : public EventsListener
	{

	public:
		System() {}

		virtual void update(ArchetypeManager& am, double t, double dt) = 0;
		virtual bool onEvent(Event& e) { return false; }
		inline const std::list<EventType>& getEventsOfInterest() const { return m_interestingEvents; };

	protected:
		std::list<EventType> m_interestingEvents;

		inline void addInterestToEvent(EventType et) { m_interestingEvents.push_back(et); }

		template<class C>
		size_t getSize(ArchetypeManager& am, size_t archId)
		{
			return am.getNbComponents<C>(archId);
		}

		std::vector<Entity>* getEntities(ArchetypeManager& am, size_t archId)
		{
			return am.getEntities(archId);
		}

		template<class C>
		C* get(ArchetypeManager& am, size_t archId)
		{
			return am.getComponents<C>(archId);
		}

	};

}

