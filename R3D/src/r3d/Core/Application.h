#pragma once

#include <R3D/Core/Window.h>
#include <R3D/Events/EventsListener.h>
#include <R3D/Events/EventManager.h>
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/Ecs/EntityManager.h>
#include <R3D/utils/RingBuffer.h>
#include <R3D/Core/Layer.h>
#include <R3D/ImGui/ImGuiLayer.h>

#include <R3D/Events/KeyEvent.h>

#ifdef  R3D_DEBUG
#define R3D_DEBUG_MSG(str) do { if(Application::isInDebugMode()) {R3D_CORE_TRACE("{0}", str);} } while( false )
#else
#define R3D_DEBUG_MSG(str) do { } while ( false )
#endif

namespace r3d
{

	class Application : public EventsListener
	{
	public:
		Application(std::string applicationName, double windowWidth, double windowHeight, int maxFramerate = 60, bool saveFrames = false);
		~Application();

		//static std::unordered_map<std::string, Entity>* getCommonEntities()
		//{
		//	return &(s_instance->m_commonEntities);
		//}

		static bool isInDebugMode()
		{
			return s_instance->m_debugMode;
		}

		static EntityManager* getEntityManager()
		{
			return &(s_instance->m_entityManager);
		}

		static Window* getWindow()
		{
			return &(s_instance->m_window);
		}

		static void dispatchEventStatic(Event& e)
		{
			s_instance->dispatchEvent(e);
		}

		//!< runs the application, updates the layers.
		void run();

		//!< pause the application.
		static void pause()
		{
			s_instance->m_paused = true;
			s_instance->m_stepping = false;

			for (auto layer : s_instance->m_layers)
			{
				std::cout << "frame cnt modulo: " << layer->getFrameSlotIndex() << std::endl;
				break;
			}
		}

	protected:
		void dispatchEvent(Event& e)
		{
			bool handled = false;

			for (auto& l : m_layers)
			{
				handled = m_eventManager.dispatch(e, l->getListeners());
				if (handled) break;
			}

			if (!handled)
			{
				handled = m_eventManager.dispatch(e, m_basicListeners);
			}

		}

		//!< Sets the application input layer, used for controlling the application (pausing, stepping, exiting, ...)
		//!< The layer is owned by the application.
		void setApplicationInputLayer(Layer* layer)
		{
			m_applicationInputLayer = layer;
		}

		//!< Adds a layer on the back. The layer is now owned by the application.
		void pushBackLayer(Layer* layer)
		{
			m_layers.push_back(layer);
			m_basicListeners[EntityDestroyedEvent::getStaticType()].push_back(layer->getArchetypeManager());
			m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_back(layer->getArchetypeManager());
		}

		//!< Adds a layer on top. The layer is now owned by the application.
		void pushFrontLayer(Layer* layer)
		{
			m_layers.push_front(layer);
			m_basicListeners[EntityDestroyedEvent::getStaticType()].push_front(layer->getArchetypeManager());
			m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_front(layer->getArchetypeManager());
		}

		virtual bool onEvent(Event& e) override;

		//!< Set debug mode. If set to true, debug prints will be called, if DEBUG_APP is also defined.
		void setDebugMode(bool debug) { m_debugMode = debug; }

	private:
		bool onKeyPressedEvent(KeyPressedEvent e);

		void stepBackLayers();
		void stepForwardLayers();

	protected:
		Window		      m_window;
		GLFWwindow*		      window;
		EntityManager     m_entityManager;
		EventManager      m_eventManager;

		Layer* m_applicationInputLayer;
		std::list<Layer*>    m_layers;
		ImGuiLayer m_imGuiLayer;

		EventsToListenersMap m_basicListeners;

		bool m_running;
		bool m_paused;
		bool m_stepping;
		bool m_debugMode;
		bool m_saveFrames;

		static Application* s_instance;

		//std::unordered_map<std::string, Entity>  m_commonEntities;
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