#pragma once

#include <R3D/Core/Window.h>
#include <R3D/Events/EventsListener.h>
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/utils/RingBuffer.h>

#include <unordered_map>

#define SAVED_FRAMES 20

namespace r3d
{

	class Layer : public EventsListener
	{
	public:
		Layer() : m_savedFrames(SAVED_FRAMES)
		{
			static int layerCounter = 0;
			layerCounter++;
			m_name = "Layer_";
			m_name.append(std::to_string(layerCounter));
		}

		Layer(const std::string& name) : m_name(name), m_savedFrames(SAVED_FRAMES)
		{
		}

		virtual ~Layer() = default;

		void onSave(Window* window);

		virtual void onUpdate(Window* window) = 0;
		virtual void onRender(Window* window) = 0;
		virtual bool onEvent(Event& e) override = 0;
		virtual void onImGuiUpdate(Window* window) {};

		inline r3d::ArchetypeManager* getArchetypeManager() { return m_savedFrames.getCurrent(); }
		inline const std::string& getName() const { return m_name; }

		inline const EventsToListenersMap& getListeners() const { return m_eventsListeners; }
		inline void pushBackLayerListener(EventType et, EventsListener* el) { m_eventsListeners[et].push_back(el); }
		inline void pushFrontLayerListener(EventType et, EventsListener* el) { m_eventsListeners[et].push_front(el); }

		void stepBack();
		void stepForward();
		virtual void onStepBack(Window* window) {};
		virtual void onStepForward(Window* window) {};

		int getFrameSlotIndex() const { return m_savedFrames.getActiveSlotIndex(); }

	protected:
		std::string            m_name;
		EventsToListenersMap   m_eventsListeners;
		RingBuffer<r3d::ArchetypeManager> m_savedFrames;

	};

}
