#pragma once

#include "Event.h"

namespace r3d
{

	class KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return m_keycode; }

	protected:
		KeyEvent(int keycode) : m_keycode(keycode) {}
		int m_keycode;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_repeatCount(repeatCount) {}

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input);
		EVENT_CLASS_TYPE(EventType::KeyPressed)
			inline int getRepeatCount() const { return m_repeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressed: " << m_keycode << ", repeated: " << m_repeatCount;
			return ss.str();
		}

	private:
		int m_repeatCount;
	};

	class KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int keycode) : KeyEvent(keycode) {}

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input);
		EVENT_CLASS_TYPE(EventType::KeyReleased)


			std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyRelease: " << m_keycode << ".";
			return ss.str();
		}

	};


	class MouseButtonPressedEvent : public KeyEvent
	{
	public:
		MouseButtonPressedEvent(int keycode, int repeatCount, float x, float y) : KeyEvent(keycode), m_repeatCount(repeatCount), m_x(x), m_y(y) {}
		MouseButtonPressedEvent(int keycode, int repeatCount, std::pair<float, float> position) : KeyEvent(keycode), m_repeatCount(repeatCount), m_x(position.first), m_y(position.second) {}

		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);
		EVENT_CLASS_TYPE(EventType::MouseButtonPressed)

		inline std::pair<float, float> getMousePosition() const { return { m_x, m_y }; }
		inline int getRepeatCount() const { return m_repeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressed: " << m_keycode << ", at position: (" << m_x << ", " << m_y << "), repeated: " << m_repeatCount;
			return ss.str();
		}

	private:
		float m_x;
		float m_y;
		int   m_repeatCount;
	};


	class MouseButtonReleasedEvent : public KeyEvent
	{
	public:
		MouseButtonReleasedEvent(int keycode, float x, float y) : KeyEvent(keycode), m_x(x), m_y(y) {}
		MouseButtonReleasedEvent(int keycode, std::pair<float, float> position) : KeyEvent(keycode), m_x(position.first), m_y(position.second) {}

		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);
		EVENT_CLASS_TYPE(EventType::MouseButtonReleased);

		inline std::pair<float, float> getMousePosition() const { return { m_x, m_y }; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleased: " << m_keycode << ", at position: (" << m_x << ", " << m_y << ").";
			return ss.str();
		}

	private:
		float m_x;
		float m_y;
	};

}