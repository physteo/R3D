#include <R3Dpch.h>

#include "Layer.h"
#include "Window.h"

namespace r3d
{

	Layer::Layer() : m_savedFrames{ SAVED_FRAMES }
	{
		static int layerCounter = 0;
		layerCounter++;
		m_name = "Layer_";
		m_name.append(std::to_string(layerCounter));
	}

	Layer::Layer(std::string name) : m_name{ std::move(name) }, m_savedFrames{ SAVED_FRAMES }
	{
	}

	void Layer::onSave(Window* window)
	{
		//m_savedFrames.saveAndAdvance();
		//std::cout << "saved in slot: " << m_frameCounterModulo << std::endl;
	}

	void Layer::stepBack()
	{
		//m_savedFrames.visitBackward();
		//std::cout << ", and now: " << m_frameCounterModulo << std::endl;
	}

	void Layer::stepForward()
	{
		//std::cout << "Layer: " << this->m_name << ": Stepping forward. Frame (modulo " <<
		//	m_framesSize << ") was: " << m_frameCounterModulo;
		//m_savedFrames.visitForward();
		//std::cout << ", and now: " << m_frameCounterModulo << std::endl;
	}

}
