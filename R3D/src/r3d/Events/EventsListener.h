#pragma once

#include <R3D/Core/Core.h>
#include "Event.h"

namespace r3d
{

	class EventsListener
	{
	public:
		virtual bool onEvent(Event& e) = 0;
	};

	template <class T>
	using ListenerList = std::list<T>;

	using EventsToListenersMap = std::unordered_map<EventType, ListenerList<EventsListener*> >;

}