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

	enum EventCategory
	{
		None = 0,
		EventCategoryInput = (1 << 1),
		EventCategoryKeyboard = (1 << 2),
		EventCategoryMouse = (1 << 3),
		EventCategoryEntity = (1 << 4),
		EventCategoryPhysics = (1 << 5),
		EventCategoryParticlePhysics = (1 << 6),
		EventCategoryRigidBodyPhysics = (1 << 7),
		EventCategoryCollisionDetection = (1 << 8),
		EventCategoryLogic = (1 << 9)
	};

}
