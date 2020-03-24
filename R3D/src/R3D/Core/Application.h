#pragma once

#include <R3D/Core/Window.h>
#include <R3D/Events/EventsListener.h>
#include <R3D/Events/EventManager.h>
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/Ecs/EntityManager.h>
#include <R3D/Utils/RingBuffer.h>
#include <R3D/Core/Layer.h>
#include <R3D/ImGui/ImGuiLayer.h>

#include <R3D/Events/KeyEvent.h>
#include <R3D/Utils/Debugger.h>

namespace r3d
{

	class Application : public EventsListener
	{
	public:
		Application(std::string applicationName, double windowWidth, double windowHeight, int maxFramerate = 60, bool saveFrames = false);
		~Application();

		static EntityManager* getEntityManager();
		static Window* getWindow();
		static Debugger* getDebugger();
		//!< Dispatches the event to all the layers and to the basic listeners.
		static void dispatchEventStatic(Event& e);
#ifdef R3D_DEBUG_APP
		static bool isInDebugMode();
#endif

		void run();
		static void pause();

	protected:
		void dispatchEvent(Event& e);
		//!< Adds a layer on the back. The layer is now owned by the application.
		void pushBackLayer(Layer* layer);
		//!< Adds a layer on top. The layer is now owned by the application.
		void pushFrontLayer(Layer* layer);

		virtual bool onEvent(Event& e) override;
#ifdef R3D_DEBUG_APP
		//!< Set debug mode. If set to true, debug prints will be called, if DEBUG_APP is also defined.
		void setDebugMode(bool debug) { m_debugMode = debug; }

		bool m_debugMode;
#endif

	private:
		bool onKeyPressedEvent(KeyPressedEvent e);
		void stepBackLayers();
		void stepForwardLayers();

		Window m_window;
		EntityManager m_entityManager;
		EventManager m_eventManager;
		Debugger m_debugger;

		std::list<Layer*>    m_layers;
		ImGuiLayer m_imGuiLayer;

		EventsToListenersMap m_basicListeners;

		bool m_running{ true };
		bool m_paused{ false };
		bool m_stepping{ false };
		bool m_saveFrames;

		static Application* s_instance;

	};

	Application* createApplication();

}

//struct ApplicationData
//{
//public:
//	ApplicationData()
//	{
//		m_applicationInputLayer = nullptr;
//	}
//
//	~ApplicationData();
//	ApplicationData(const ApplicationData& other);
//	ApplicationData& operator=(const ApplicationData& other);
//
//	EntityManager     m_entityManager;
//	EventManager      m_eventManager;
//	EventsToListenersMap m_basicListeners;
//
//	std::list<Layer*>    m_layers;
//	Layer* m_applicationInputLayer;
//
//};
//
//class ApplicationDataRingBuffer
//{
//	public:
//		ApplicationDataRingBuffer(size_t size)
//		{
//			m_size = size;
//			m_data.resize(size);
//			m_index = 0;
//		}
//
//		inline ApplicationData* getCurrent() { return &m_currentData; }
//		void saveCurrent()
//		{
//			increaseIndex();
//			m_data[m_index] = m_currentData;
//		}
//
//	protected:
//		void increaseIndex() { (m_index + 1) % m_size; }
//		void decreaseIndex() { (m_index + m_size - 1) % m_size; }
//
//	private:
//		ApplicationData              m_currentData;
//		std::vector<ApplicationData> m_data;
//		size_t m_size;
//		size_t m_index;
//};
