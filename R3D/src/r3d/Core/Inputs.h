#pragma once

/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* maths */
#include "../../utils/math.h"

/* Engine */
#include "./Window.h"
#include "../../core/events/KeyEvent.h"
#include "../../core/Application.h"


class Input
{
public:
	static Input& get()
	{
		static Input instance;
		return instance;
	}

	Input(const Input&) = delete;
	void operator=(const Input&) = delete;

	bool isKeyPressed(int key, Window& window)
	{
		auto state = glfwGetKey(window.getGLFWwindow(), key);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	bool isKeyRelease(int key, Window& window)
	{
		auto state = glfwGetKey(window.getGLFWwindow(), key);
		return (state == GLFW_RELEASE);
	}

	bool isMouseButtonPressed(int button, Window& window)
	{
		auto state = glfwGetMouseButton(window.getGLFWwindow(), button);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}
	bool isMouseButtonReleased(int button, Window& window)
	{
		auto state = glfwGetMouseButton(window.getGLFWwindow(), button);
		return (state == GLFW_RELEASE);
	}

	std::pair<float, float> getMousePosition(Window& window)
	{
		double xpos, ypos;
		glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

private:
	Input() {};
};



struct KeyInfo
{
	KeyInfo() : keyCode(0), justPressed(false),
		keyRepeatCount(0), firstPressTime(0.0f) {}
	KeyInfo(int keyCodeIn) : keyCode(keyCodeIn), justPressed(false),
		keyRepeatCount(0), firstPressTime(0.0f) {}

	int   keyCode;
	bool  justPressed;
	int   keyRepeatCount;
	float firstPressTime;
};


struct MouseButtonInfo
{
	MouseButtonInfo() : keyCode(0), justPressed(false),
		keyRepeatCount(0), firstPressTime(0.0f) {}
	MouseButtonInfo(int keyCodeIn) : keyCode(keyCodeIn), justPressed(false),
		keyRepeatCount(0), firstPressTime(0.0f) {}

	int   keyCode;
	bool  justPressed;
	int   keyRepeatCount;
	float firstPressTime;
	float x;
	float y;
};

class InputProcessor
{
public:
	InputProcessor(std::vector<KeyInfo> keysIn, std::vector<MouseButtonInfo> mouseButtonsIn);

	void processInputs(Window& window);

private:
	std::vector<KeyInfo> keys;
	std::vector<MouseButtonInfo> mouseButtons;

	bool processPressed(KeyInfo& key, Window& window);
	bool processRelease(KeyInfo& key, Window& window);

	bool processMousePressed(MouseButtonInfo& mouseButton, Window& window);
	bool processMouseRelease(MouseButtonInfo& mouseButton, Window& window);

};
