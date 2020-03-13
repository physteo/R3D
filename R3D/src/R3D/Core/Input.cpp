#include <R3Dpch.h>

#include "Input.h"

namespace r3d
{

	Input& Input::getInstance()
	{
		static Input instance;
		return instance;
	}

	bool Input::isKeyPressed(int key, Window& window)
	{
		auto state = glfwGetKey(window.getGLFWwindow(), key);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	bool Input::isKeyRelease(int key, Window& window)
	{
		auto state = glfwGetKey(window.getGLFWwindow(), key);
		return (state == GLFW_RELEASE);
	}

	bool Input::isMouseButtonPressed(int button, Window& window)
	{
		auto state = glfwGetMouseButton(window.getGLFWwindow(), button);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}
	bool Input::isMouseButtonReleased(int button, Window& window)
	{
		auto state = glfwGetMouseButton(window.getGLFWwindow(), button);
		return (state == GLFW_RELEASE);
	}

	std::pair<float, float> Input::getMousePosition(Window& window)
	{
		double xpos, ypos;
		glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

}