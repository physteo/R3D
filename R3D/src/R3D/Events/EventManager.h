#pragma once

#include <R3D/Core/Core.h>
#include "Event.h"
#include "EventsListener.h"

namespace r3d
{

	class R3D_API EventManager
	{
	public:
		bool dispatch(Event& e, const EventsToListenersMap& listeners);
	};

}