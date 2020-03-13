#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <R3D/Core/Window.h>

namespace r3d
{

	class Input
	{
	public:
		static Input& getInstance();

		Input(const Input&) = delete;
		void operator=(const Input&) = delete;

		bool isKeyPressed(int key, Window& window);
		bool isKeyRelease(int key, Window& window);
		bool isMouseButtonPressed(int button, Window& window);
		bool isMouseButtonReleased(int button, Window& window);

		std::pair<float, float> getMousePosition(Window& window);

	private:
		Input() {};
	};

}
