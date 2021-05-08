#pragma once
#include <R3D/Core/Core.h>
#include "EventTypesAndCategories.h"

namespace r3d
{

#define EVENT_CLASS_TYPE(type)	static  EventType getStaticType() {return type;}\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual EventCategory getCategoryFlags() const override {return category;}

	class Event
	{
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName()    const = 0;
		virtual EventCategory getCategoryFlags()   const = 0;

		virtual std::string toString()   const { return getName(); }

		inline bool isInCategory(enum class EventCategory category) const
		{
			return static_cast<int>(getCategoryFlags()) & static_cast<int>(category);
		}

		bool handled = false;

	};

	/*
		In the Listener class that receives an event, we create an EventDispatcher class that wraps that event's reference.
		We use the dispatch method of this class with a function (usually defined in the listener). The dispatcher will try
		to apply this function on the event, by checking if their type match.
		This is useful because in our onEvent function in the listener, we dont have to do "if event matches, do this ..."
		for all kinds of events.
	*/
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : m_event(event)
		{
		}

		template<typename T>
		bool dispatch(EventFn<T> func)
		{
			if (m_event.getEventType() == T::getStaticType())
			{
				m_event.handled = func(*(T*)&m_event);
				return m_event.handled;
			}
			return false;
		}
	private:
		Event& m_event;
	};

#define HANDLE_DISPATCH(dispatcher, eventClass, function) if( dispatcher.dispatch<eventClass>(std::bind(&function, this, std::placeholders::_1)) == true ) { return true; }

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}

}
