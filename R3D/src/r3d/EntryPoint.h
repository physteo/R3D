#pragma once

#include <R3D/Core.h>
#include <R3D/Log/Log.h>

#ifdef R3D_PLATFORM_WIN
	
extern r3d::Application* r3d::createApplication();

int main(int argc, char** argv)
{
	R3D_CORE_INFO("Rendara3D Engine.");
	r3d::Log::init();

	auto app = r3d::createApplication();
	app->run();
	delete app;
}

#endif
