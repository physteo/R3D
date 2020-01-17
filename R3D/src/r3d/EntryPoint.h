#pragma once

#ifdef R3D_PLATFORM_WIN
	
extern r3d::Application* r3d::createApplication();

	int main(int argc, char** argv)
	{
		std::cout << "Rendara3D Engine." << std::endl;
		auto app = r3d::createApplication();
		app->run();
		delete app;
	}

#endif
