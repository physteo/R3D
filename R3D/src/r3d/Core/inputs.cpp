#include <R3Dpch.h>
#include "Inputs.h"

InputProcessor::InputProcessor(std::vector<KeyInfo> keysIn, std::vector<MouseButtonInfo> mouseButtonsIn)
	: keys(keysIn), mouseButtons(mouseButtonsIn) {}


void InputProcessor::processInputs(Window& window)
{
	for (auto& key : keys)
	{
		if (processPressed(key, window))
		{
			KeyPressedEvent keyPressed{ key.keyCode, key.keyRepeatCount };
			Application::dispatchEventStatic(keyPressed);
#if 0
			std::cout << "Pressed " << key.keyCode << ", count: " << key.keyRepeatCount << std::endl;
#endif
		}
	}
	for (auto& key : keys)
	{
		if (processRelease(key, window))
		{
			KeyReleaseEvent keyRelease{ key.keyCode };
			Application::dispatchEventStatic(keyRelease);
#if 0
			std::cout << "Release " << key.keyCode << "." << std::endl;
#endif
		}
	}

	for (auto& button : mouseButtons)
	{
		if (processMousePressed(button, window))
		{
			MouseButtonPressedEvent mousePressed{ button.keyCode, button.keyRepeatCount, {button.x, button.y} };
			Application::dispatchEventStatic(mousePressed);
#if 0
			std::cout << "Pressed " << button.keyCode << ", position: (" <<
				 button.x << "," << button.y <<
				"), count: " << button.keyRepeatCount << std::endl;
#endif
		}
	}

	for (auto& button : mouseButtons)
	{
		if (processMouseRelease(button, window))
		{
			MouseButtonReleasedEvent buttonRelease{ button.keyCode, {button.x, button.y} };
			Application::dispatchEventStatic(buttonRelease);
#if 0
			std::cout << "Released " << button.keyCode << ", position: (" <<
				button.x << "," << button.y << ")." << std::endl;
#endif
		}
	}

}



bool InputProcessor::processPressed(KeyInfo& key, Window& window)
{
	if (Input::get().isKeyPressed(key.keyCode, window))
	{
		bool process = false;
		if (key.justPressed == true)
		{
			++(key.keyRepeatCount);
			process = true;
		}
		else
		{
			key.firstPressTime = window.getCurrentTime();
			key.keyRepeatCount = 0;
			process = true;
		}

		key.justPressed = true;
		return process;
	}
	else
	{
		return false;
	}
}

bool InputProcessor::processRelease(KeyInfo& key, Window& window)
{
	if (Input::get().isKeyRelease(key.keyCode, window) &&  key.justPressed == true)
	{
		bool process = false;

		if (key.justPressed == true)
		{
			process = true;
		}

		key.justPressed = false;
		key.keyRepeatCount = 0;
		key.firstPressTime = 0.0f;

		return process;
	}
	return false;
}

bool InputProcessor::processMousePressed(MouseButtonInfo& mouseButton, Window & window)
{
	if (Input::get().isMouseButtonPressed(mouseButton.keyCode, window))
	{
		bool process = false;
		if (mouseButton.justPressed == true)
		{
			++(mouseButton.keyRepeatCount);
			process = true;
		}
		else
		{
			mouseButton.firstPressTime = window.getCurrentTime();
			mouseButton.keyRepeatCount = 0;
			process = true;
		}

		auto xy = Input::get().getMousePosition(window);
		mouseButton.x = xy.first;
		mouseButton.y = xy.second;
		mouseButton.justPressed = true;
		return process;
	}
	else
	{
		return false;
	}
}

bool InputProcessor::processMouseRelease(MouseButtonInfo& mouseButton, Window & window)
{
	if (Input::get().isMouseButtonReleased(mouseButton.keyCode, window) && mouseButton.justPressed == true)
	{
		bool process = false;
	
		if (mouseButton.justPressed == true)
		{
			process = true;
		}
	
		mouseButton.justPressed = false;
		mouseButton.keyRepeatCount = 0;
		mouseButton.firstPressTime = 0.0f;
	
		return process;
	}
	return false;
}
