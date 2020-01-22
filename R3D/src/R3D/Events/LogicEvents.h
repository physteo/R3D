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


	//class ChangeColorEvent : public Event
	//{
	//public:
	//	ChangeColorEvent(Entity entityIn, glm::vec3 colorIn) : m_entity(entityIn), m_color(colorIn) {}
	//
	//	EVENT_CLASS_TYPE(EventType::Logic_ChangeColor);
	//	EVENT_CLASS_CATEGORY(EventCategoryLogic);
	//
	//	inline Entity getEntity() const { return m_entity; }
	//	inline glm::vec3 getColor() const { return m_color; }
	//	std::string toString() const override
	//	{
	//		std::stringstream ss;
	//		ss << "Color (Entity: " << m_entity.id << ") changed to " << m_color.r << "," << m_color.g << "," << m_color.b << ".";
	//		return ss.str();
	//	}
	//private:
	//	Entity m_entity;
	//	glm::vec3 m_color;
	//};
}