#pragma once

#include "Entity.h"
#include <R3D/Events/Event.h>

#include <sstream>
#include <queue>

namespace r3d
{

	class EntityDestroyedEvent : public Event
	{
	public:
		EntityDestroyedEvent() : m_entity() {}
		EntityDestroyedEvent(Entity entity) : m_entity(entity) {}

		EVENT_CLASS_CATEGORY(EventCategoryEntity);
		EVENT_CLASS_TYPE(EventType::Entities_EntityDestroyed)

			inline Entity getEntity() const { return m_entity; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Entity destroyed. ID: " << m_entity.id;
			return ss.str();
		}

	private:
		Entity m_entity;
	};

	class ManyEntitiesDestroyedEvent : public Event
	{
	public:
		ManyEntitiesDestroyedEvent(std::vector<Entity> entitiesIn) : entities(std::move(entitiesIn)) {}

		EVENT_CLASS_CATEGORY(EventCategoryEntity);
		EVENT_CLASS_TYPE(EventType::Entities_ManyEntitiesDestroyedEvent)

			//inline Entity getEntity() const { return m_entity; }

			std::string toString() const override
		{
			std::stringstream ss;
			ss << "Many entities destroyed. Ids: ";// << m_entity.id;
			for (auto it = entities.begin(); it != entities.end(); ++it) ss << it->id << ", ";
			return ss.str();
		}

		std::vector<Entity> entities;
	};

	class EntityDestroyRequestEvent : public Event
	{
	public:
		EntityDestroyRequestEvent() : m_entity() {}
		EntityDestroyRequestEvent(Entity entity) : m_entity(entity) {}

		EVENT_CLASS_CATEGORY(EventCategoryEntity);
		EVENT_CLASS_TYPE(EventType::Entities_EntityDestroyRequest)

			inline Entity getEntity() const { return m_entity; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Entity destroy request. ID: " << m_entity.id;
			return ss.str();
		}

	private:
		Entity m_entity;
	};

	class ManyEntitiesDestroyRequestEvent : public Event
	{
	public:
		ManyEntitiesDestroyRequestEvent(std::vector<Entity>* entitiesIn) : entities(entitiesIn) {}

		EVENT_CLASS_CATEGORY(EventCategoryEntity);
		EVENT_CLASS_TYPE(EventType::Entities_ManyEntitiesDestroyRequest)

			//inline Entity getEntity() const { return m_entity; }

			std::string toString() const override
		{
			std::stringstream ss;
			ss << "Many entities destroy request. Ids: ";// << m_entity.id;
			for (auto it = entities->begin(); it != entities->end(); ++it) ss << it->id << ", ";
			return ss.str();
		}

		std::vector<Entity>* entities;
	};

}