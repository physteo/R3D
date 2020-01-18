#pragma once

#include <R3D/Core.h>

namespace r3d
{

	class R3D_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	};

	Application* createApplication();

}
