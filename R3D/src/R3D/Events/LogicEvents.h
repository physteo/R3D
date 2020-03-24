#pragma once

#include "./Event.h"
#include "../entities/Entity.h"
#include "EventTypesAndCategories.h"
namespace r3d
{

	class HealthDamageEvent : public Event
	{
	public:
		HealthDamageEvent(Entity entityIn, float damageIn) : entity(entityIn), damage(damageIn) {}

		EVENT_CLASS_TYPE(EventType::Logic_HealthDamage);
		EVENT_CLASS_CATEGORY(EventCategoryLogic);


		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Entity: " << entity.id << ", damaged: " << damage;
			return ss.str();
		}

		Entity entity;
		float damage;
	};


	class ButtonPressedEvent : public Event
	{
	public:
		ButtonPressedEvent(std::string buttonName) : m_buttonName(buttonName) {}

		EVENT_CLASS_TYPE(EventType::Logic_ButtonPressed);
		EVENT_CLASS_CATEGORY(EventCategoryLogic);

		inline const std::string& getButtonName() { return m_buttonName; }
		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Button '" << m_buttonName << "' pressed.";
			return ss.str();
		}
	private:
		std::string m_buttonName;
	};

}