#include <R3Dpch.h>
#include "Layer.h"

namespace r3d
{

	void Layer::onSave(Window* window)
	{
//		m_savedFrames.saveAndAdvance();
		//std::cout << "saved in slot: " << m_frameCounterModulo << std::endl;
	}

	void Layer::stepBack()
	{
//		m_savedFrames.visitBackward();
		//std::cout << ", and now: " << m_frameCounterModulo << std::endl;
	}

	void Layer::stepForward()
	{
		//std::cout << "Layer: " << this->m_name << ": Stepping forward. Frame (modulo " <<
		//	m_framesSize << ") was: " << m_frameCounterModulo;
//		m_savedFrames.visitForward();
		//std::cout << ", and now: " << m_frameCounterModulo << std::endl;
	}

}
