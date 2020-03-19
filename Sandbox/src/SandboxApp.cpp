#include <R3D.h>
#include <R3D/Core/EntryPoint.h>

#include "WorldLayer.h"

namespace r3d
{
	 
	class Sandbox : public r3d::Application
	{
	public:
		Sandbox() : Application{ "Sandbox app.", 1600, 800, 64 }
		{
			//m_window.setFontscale(1.0);
#ifdef R3D_DEBUG_APP
			setDebugMode(false);
#endif
			pushBackLayer(new WorldLayer()); 
		}

		~Sandbox()
		{
		}
	};

	Application* createApplication()
	{
		return new Sandbox();
	}

}