#pragma once

#include <R3D/Events/EventsListener.h>
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/Utils/RingBuffer.h>

#define SAVED_FRAMES 0

namespace r3d
{
	
	class Window;

	class Layer : public EventsListener
	{
	public:
		Layer();
		Layer(std::string name);
		virtual ~Layer() = default;

		inline r3d::ArchetypeManager* getArchetypeManager() { return m_savedFrames.getCurrent(); }
		inline const std::string& getName() const { return m_name; }
		inline const EventsToListenersMap& getListeners() const { return m_eventsListeners; }
		
		inline void pushBackLayerListener(EventType et, EventsListener* el) { m_eventsListeners[et].push_back(el); }
		inline void pushFrontLayerListener(EventType et, EventsListener* el) { m_eventsListeners[et].push_front(el); }

		virtual void onUpdate(Window* window) = 0;
		virtual void onRender(Window* window) = 0;
		virtual bool onEvent(Event& e) override = 0;
		virtual void onImGuiUpdate(Window* window) {};
		void onSave(Window* window);

		void stepBack();
		void stepForward();
		virtual void onStepBack(Window* window) {};
		virtual void onStepForward(Window* window) {};

		size_t getFrameSlotIndex() const { return m_savedFrames.getActiveSlotIndex(); }

	protected:
		std::string            m_name;
		EventsToListenersMap   m_eventsListeners;
		RingBuffer<r3d::ArchetypeManager> m_savedFrames;

	};

}
