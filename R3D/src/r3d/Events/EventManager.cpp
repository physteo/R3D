#include <R3Dpch.h>
#include "EventManager.h"

namespace r3d
{

	bool EventManager::dispatch(Event& e, const EventsToListenersMap& listeners)
	{
		auto found = listeners.find(e.getEventType());
		if (found != listeners.end())
		{
			for (auto& listener : found->second)
			{
#ifdef R3D_DEBUG_EVENTS
				std::cout << "Handling Event: " << e.getName() << ", Handler: " << listener << std::endl;
				std::cout << "\t Event Info: " << e.toString() << std::endl;
#endif
				if (listener->onEvent(e))
				{
					break;
				}
			}
		}

		return e.handled;
	}

}