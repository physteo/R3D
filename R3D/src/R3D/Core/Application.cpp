#include <R3Dpch.h>
#include "Application.h"
#include <R3D/ImGui/ImGuiLayer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace r3d
{

	Application* Application::s_instance = nullptr;

	EntityManager* Application::getEntityManager()
	{
		return &(s_instance->m_entityManager);
	}

	Window* Application::getWindow()
	{
		return &(s_instance->m_window);
	}

	Debugger* Application::getDebugger()
	{
		return &(s_instance->m_debugger);
	}

	void Application::dispatchEventStatic(Event& e)
	{
		s_instance->dispatchEvent(e);
	}

#ifdef R3D_DEBUG_APP
	bool Application::isInDebugMode()
	{
		return s_instance->m_debugMode;
	}
#endif

	Application::Application(std::string applicationName, double windowWidth, double windowHeight, int maxFramerate, bool saveFrames)
		: m_window{ applicationName, windowWidth, windowHeight, maxFramerate }//m_dataRingBuffer(60)
	{
		s_instance = this;
		m_saveFrames = saveFrames;

		// The EntityManager listens to events regarding destruction of entities
		m_basicListeners[EntityDestroyedEvent::getStaticType()].push_back(&m_entityManager);
		m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_back(&m_entityManager);

		// The Application itself listens to some events.
		m_basicListeners[KeyPressedEvent::getStaticType()].push_back(this);

#ifdef R3D_DEBUG_APP
		m_debugMode = true;
#endif
	}

	Application::~Application()
	{
		for (auto& layer : m_layers)
		{
			delete layer;
		}
	}

	void Application::run()
	{
		m_imGuiLayer.init(&m_window);

		while (!m_window.isClosed() && m_running)
		{
			if (true) // TODO: reimplement pausing
			{
				// ******* initialize frame
				auto start = std::chrono::high_resolution_clock::now();
				m_window.updateTime();
				m_window.clearColorBufferBit();

				// ******* update, render and save layers
				for (auto& layer : m_layers)
				{
					layer->onUpdate(&m_window);
					layer->onRender(&m_window);
					if (m_saveFrames)
					{
						layer->onSave(&m_window);
					}
				}
				
				// ****** imgui update and render
				m_imGuiLayer.onUpdate(&m_window);
				for (auto& layer : m_layers)
				{
					layer->onImGuiUpdate(&m_window);
				}
				m_imGuiLayer.onRender(&m_window);
				
				// ******* last stuff to do
				m_window.swapBuffers();
				m_window.updateLastFrameTime();

				// **********  fix maximum framerate
				auto end = std::chrono::high_resolution_clock::now();
				long long durationNs = (end - start).count();
				long long fixedDuration = (long long)(1000000000.0f / m_window.getMaxframerate());
				long long waitingTime = fixedDuration - durationNs;
				if (waitingTime > 0)
				{
					std::this_thread::sleep_for(std::chrono::nanoseconds::duration(waitingTime));
					m_window.updateLastFrameTime();
				}
				//R3D_CORE_INFO("duration: {0}", durationMs);
				//R3D_CORE_INFO("waiting: {0}", waitingTime);
			}
			else
			{
				// perform only rendering during pause
				m_window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);
				for (auto& layer : m_layers)
				{
					layer->onRender(&m_window);
				}
				m_window.swapBuffers();
			}

			if (m_stepping)
			{
				m_paused = true;
			}

			m_window.pollEvents();
		}

		m_imGuiLayer.shutdown();
	}

	void Application::pause()
	{
		s_instance->m_paused = true;
		s_instance->m_stepping = false;

		for (auto layer : s_instance->m_layers)
		{
			std::cout << "frame cnt modulo: " << layer->getFrameSlotIndex() << std::endl;
			break;
		}
	}

	void Application::dispatchEvent(Event& e)
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

	void Application::pushBackLayer(Layer* layer)
	{
		m_layers.push_back(layer);
		m_basicListeners[EntityDestroyedEvent::getStaticType()].push_back(layer->getArchetypeManager());
		m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_back(layer->getArchetypeManager());
	}

	void Application::pushFrontLayer(Layer* layer)
	{
		m_layers.push_front(layer);
		m_basicListeners[EntityDestroyedEvent::getStaticType()].push_front(layer->getArchetypeManager());
		m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_front(layer->getArchetypeManager());
	}

	bool Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		HANDLE_DISPATCH(dispatcher, KeyPressedEvent, Application::onKeyPressedEvent);
		return true;
	}

	bool Application::onKeyPressedEvent(KeyPressedEvent e)
	{
		if (e.getKeyCode() == GLFW_KEY_ENTER && e.getRepeatCount() == 0)
		{
			if (m_paused)
			{
				m_paused = false;
				m_stepping = false;
			}
			else
			{
				this->pause();
			}
			return true;
		}
		if (e.getKeyCode() == GLFW_KEY_TAB && e.getRepeatCount() == 0)
		{
			m_paused = false;
			m_stepping = true;
		}
		if (e.getKeyCode() == GLFW_KEY_A && e.getRepeatCount() == 0)
		{
			if (m_paused)
			{
				stepBackLayers();
			}
		}
		else if (e.getKeyCode() == GLFW_KEY_D && e.getRepeatCount() == 0)
		{
			if (m_paused)
			{
				stepForwardLayers();
			}
		}
#ifdef R3D_DEBUG_APP
		if (e.getKeyCode() == GLFW_KEY_DELETE && e.getRepeatCount() == 0)
		{
			m_debugMode = !m_debugMode;
			std::cout << "Debug mode: " << m_debugMode << std::endl;
		}
#endif
		return false;
	}

	void Application::stepBackLayers()
	{
		m_window.clearColorBufferBit();
		for (auto layer : m_layers)
		{
			layer->stepBack();
			layer->onStepBack(&m_window);
			layer->onRender(&m_window);
		}
		m_window.swapBuffers();

	}

	void Application::stepForwardLayers()
	{
		m_window.clearColorBufferBit();
		for (auto layer : m_layers)
		{
			layer->stepForward();
			layer->onStepForward(&m_window);
			layer->onRender(&m_window);
		}
		m_window.swapBuffers();

	}

}

//ApplicationData::~ApplicationData()
//{
//	for (auto& layer : m_layers)
//	{
//		delete layer;
//	}
//	delete m_applicationInputLayer;
//}
//
//ApplicationData::ApplicationData(const ApplicationData& other)
//{
//	m_entityManager = other.m_entityManager;
//	m_eventManager = other.m_eventManager;
//	m_basicListeners = other.m_basicListeners;
//
//	for (auto& layer : other.m_layers)
//	{
//		this->m_layers.push_back(layer->clone());
//	}
//	m_applicationInputLayer = other.m_applicationInputLayer->clone();
//}
//
//ApplicationData& ApplicationData::operator=(const ApplicationData& other)
//{
//	if (this != &other)
//	{
//		m_entityManager = other.m_entityManager;
//		m_eventManager = other.m_eventManager;
//		m_basicListeners = other.m_basicListeners;
//
//		for (auto& layer : other.m_layers)
//		{
//			this->m_layers.push_back(layer->clone());
//		}
//		m_applicationInputLayer = other.m_applicationInputLayer->clone();
//	}
//	return *this;
//}