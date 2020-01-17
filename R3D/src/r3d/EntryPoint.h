#pragma once

#ifdef R3D_PLATFORM_WIN
	
extern r3d::Application* r3d::createApplication();

	int main(int argc, char** argv)
	{
		r3d::Log::init();
		R3D_CORE_INFO("Log initialized.");
		double a = 2.000001;
		R3D_CRITICAL("Rendara3D Engine Var = {0}.", a);
		

		auto app = r3d::createApplication();
		app->run();
		delete app;
	}

#endif
