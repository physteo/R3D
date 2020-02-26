#include <R3Dpch.h>
#include "Application.h"
#include <R3D/ImGui/ImGuiLayer.h>

namespace r3d
{

	Application* Application::s_instance = nullptr;

	Application::Application(std::string applicationName, double windowWidth, double windowHeight, int maxFramerate, bool saveFrames)
		: m_window{ applicationName, windowWidth, windowHeight },	m_running(true), m_paused(false)//m_dataRingBuffer(60)
	{
		s_instance = this;
		m_saveFrames = saveFrames;

		// The EntityManager listens to events regarding destruction of entities
		m_basicListeners[EntityDestroyedEvent::getStaticType()].push_back(&m_entityManager);
		m_basicListeners[ManyEntitiesDestroyedEvent::getStaticType()].push_back(&m_entityManager);

		// The Application itself listens to some events.
		m_basicListeners[KeyPressedEvent::getStaticType()].push_back(this);

		m_applicationInputLayer = nullptr;

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
				int frameTimeMicrosec = (int)(m_window.getLastFrameTime() * 1000000.0f);
				int waitingTime = (int)(1000000.0f / m_window.getMaxframerate()) - frameTimeMicrosec;
				if (waitingTime > 0)
				{
					std::this_thread::sleep_for(std::chrono::microseconds(waitingTime));
					m_window.updateLastFrameTime();
				}
			}
			else
			{
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

			// ********** always process application inputs...
			if(m_applicationInputLayer) m_applicationInputLayer->onUpdate(&m_window);
			// **********  ... and poll events.
			m_window.pollEvents();
		}

		m_imGuiLayer.shutdown();
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
		m_window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);
		// step back all layers
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
		m_window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

		// step back all layers
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