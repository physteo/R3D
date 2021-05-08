#pragma once

namespace r3d
{

	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased,
		Entities_EntityDestroyed,
		Entities_ManyEntitiesDestroyedEvent,
		Entities_EntityDestroyRequest,
		Entities_ManyEntitiesDestroyRequest,
		ParticlePhysics_ParticleFallen,
		ParticlePhysics_ParticlesMoved,
		ParticlePhysics_ParticleIsDead,
		CollisionDetection_CircleCircle,
		CollisionDetection_CirclePlane,
		CollisionDetection_CircleEdge,
		CollisionDetection_EnterSleep,
		CollisionDetection_Awake,
		Logic_HealthDamage,
		Logic_ButtonPressed,
		Logic_ChangeColor,
	};

	enum class EventCategory
	{
		None = 0,
		Input = (1 << 1),
		Keyboard = (1 << 2),
		Mouse = (1 << 3),
		Entity = (1 << 4),
		Physics = (1 << 5),
		ParticlePhysics = (1 << 6),
		RigidBodyPhysics = (1 << 7),
		CollisionDetection = (1 << 8),
		Logic = (1 << 9)
	};

	static EventCategory operator|(EventCategory a, EventCategory b)
	{
		return static_cast<EventCategory>(static_cast<int>(a) | static_cast<int>(b));
	}

	static EventCategory operator&(EventCategory a, EventCategory b)
	{
		return static_cast<EventCategory>(static_cast<int>(a) & static_cast<int>(b));
	}
}
