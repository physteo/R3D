#pragma once

#include "../Events/Event.h"
#include "../../utils/SwapArray.h"

/* maths */
#include "../../utils/math.h"
#include "../../physics/real.h"

struct EnterSleepEvent : public Event
{
	EnterSleepEvent() = default;
	EnterSleepEvent(Entity aIn) : a(aIn) {}

	EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
	EVENT_CLASS_TYPE(EventType::CollisionDetection_EnterSleep)

		std::string toString() const override
	{
		std::stringstream ss;
		ss << "Entity " << a.id << " enters sleep.";
		return ss.str();
	}

	Entity a;
};

struct AwakeEvent : public Event
{
	AwakeEvent() = default;
	AwakeEvent(Entity aIn) : a(aIn) {}

	EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
	EVENT_CLASS_TYPE(EventType::CollisionDetection_Awake)

		std::string toString() const override
	{
		std::stringstream ss;
		ss << "Entity " << a.id << " awakes.";
		return ss.str();
	}
	Entity a;
};


struct CircleCircleCollision
{
	CircleCircleCollision() = default;
	CircleCircleCollision(Entity aIn, Entity bIn,
		const real3& pos_aIn, const real3& pos_bIn,
		//const real3& vel_aIn, const real3& vel_bIn,
		real collisionTimeIn,
		bool modifyPositionsIn)
		: a(aIn), b(bIn),
		pos_a(pos_aIn), pos_b(pos_bIn),
		//vel_a(vel_aIn), vel_b(vel_bIn), 
		collisionTime(collisionTimeIn),
		modifyPositions(modifyPositionsIn) {}
	
	Entity a;
	Entity b;
	real3 pos_a;
	real3 pos_b;
	//real3 vel_a;
	//real3 vel_b;
	real collisionTime;
	bool modifyPositions;
};

class CircleCircleCollisionsEvent : public Event
{
public:
	CircleCircleCollisionsEvent(const std::vector<CircleCircleCollision>* collisions, size_t numberOfCollisions)
		: m_collisions(collisions), m_numberOfCollisions(numberOfCollisions) {}

	EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
	EVENT_CLASS_TYPE(EventType::CollisionDetection_CircleCircle)

		std::string toString() const override
	{
		std::stringstream ss;
		ss << "Circle/Circle event list.";
		return ss.str();
	}

	inline const std::vector<CircleCircleCollision>* getCollisionsVector() const { return m_collisions; }

private:
	const std::vector<CircleCircleCollision>* m_collisions;
	size_t m_numberOfCollisions;
};

struct CircleEdgeCollision
{
	CircleEdgeCollision() = default;
	CircleEdgeCollision(Entity circleIn, Entity edgeIn,
		const real3& pos_aIn, const real3& pos_bIn, const real3& pos_cIn,
		const real3& vel_aIn, const real3& vel_bIn, const real3& vel_cIn,
		const real3& normalIn,
		bool modifyPositionsIn) : 
		circle(circleIn), edge(edgeIn),
		pos_a(pos_aIn), pos_b(pos_bIn), pos_c(pos_cIn),
		vel_a(vel_aIn), vel_b(vel_bIn), vel_c(vel_cIn),
		normal(normalIn),
		modifyPositions(modifyPositionsIn){}

	Entity circle;
	Entity edge;

	real3 pos_a;
	real3 pos_b;
	real3 pos_c;

	real3 vel_a;
	real3 vel_b;
	real3 vel_c;

	real3 normal;

	bool modifyPositions;
};

class CircleEdgeCollisionsEvent : public Event
{
public:
	CircleEdgeCollisionsEvent(const std::vector<CircleEdgeCollision>* collisions, size_t numberOfCollisions)
		: m_collisions(collisions), m_numberOfCollisions(numberOfCollisions) {}

	EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
	EVENT_CLASS_TYPE(EventType::CollisionDetection_CircleEdge)

		std::string toString() const override
	{
		std::stringstream ss;
		ss << "Circle/Edge event list.";
		return ss.str();
	}

	inline const std::vector<CircleEdgeCollision>* getCollisionsVector() const { return m_collisions; }

private:
	const std::vector<CircleEdgeCollision>* m_collisions;
	size_t m_numberOfCollisions;
};

struct CirclePlaneCollision
{
	CirclePlaneCollision() = default;
	CirclePlaneCollision(Entity circleIn, Entity planeIn,
		const real3& planeSurfacePositionIn, const real3& planeNormalIn,
		const real3& spherePositionIn,
		real collisionTimeIn,
		bool modifySpherePositionIn)
		: circle(circleIn), plane(planeIn),
		planeSurfacePosition(planeSurfacePositionIn), planeNormal(planeNormalIn),
		spherePosition(spherePositionIn),
		collisionTime(collisionTimeIn),
		modifySpherePosition(modifySpherePositionIn) {}
	Entity circle;
	Entity plane;
	real3 planeSurfacePosition;
	real3 planeNormal;
	real3 spherePosition;
	real collisionTime;
	bool modifySpherePosition;
};

class CirclePlaneCollisionsEvent : public Event
{
public:
	CirclePlaneCollisionsEvent(const std::vector<CirclePlaneCollision>* collisions, size_t numberOfCollisions)
		: m_collisions(collisions), m_numberOfCollisions(numberOfCollisions) {}

	EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
	EVENT_CLASS_TYPE(EventType::CollisionDetection_CirclePlane)

		std::string toString() const override
	{
		std::stringstream ss;
		ss << "Circle/Plane event list.";
		return ss.str();
	}

	inline const std::vector<CirclePlaneCollision>* getCollisionsVector() const { return m_collisions; }

private:
	const std::vector<CirclePlaneCollision>* m_collisions;
	size_t m_numberOfCollisions;
};
