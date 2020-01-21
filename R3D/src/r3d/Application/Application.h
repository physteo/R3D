#pragma once

#include <R3D/Core.h>

namespace r3d
{

	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run();
	};

	Application* createApplication();

}

