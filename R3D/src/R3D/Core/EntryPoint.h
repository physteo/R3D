#pragma once

#include <R3D/Core/Core.h>
#include <R3D/Core/Log.h>

	
extern r3d::Application* r3d::createApplication();

int main(int argc, char** argv)
{
	r3d::Log::init();
	R3D_CORE_INFO("Rendara3D Engine.");

	auto app = r3d::createApplication();
	app->run();
	delete app;
}

