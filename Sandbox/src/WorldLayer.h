#pragma once
#include <R3D.h>

#include <R3D/Physics/Core/Colliders.h>
#include <R3D/Physics/Core/Topology.h>
#include <R3D/Utils/Debugger.h>
#include <R3D/Physics/Core/World.h>
#include <R3D/Physics/Systems/Integrator.h>

#include "SolidPrimitivesRenderer.h"
#include "Palette.h"

#define STACKING

namespace r3d
{

	const float stdGravity{-10.0f};

	struct WallTag { };
	struct BrickTag {};
	struct WoodTag {};
	struct BulletTag {};
	struct GroundTag {};

	struct BloomSettings
	{
		bool on{ true };
		float intensity{ 0.275f };
		float blurOffset{ 1.75f };
		float blurFalloff[5]{ 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
		int   blurPasses{ 4 };
		float3 invThreshold{ 0.5f * 0.2126f, 0.5f * 0.7152f, 0.5f * 0.0722f };
	};

	struct HDRSettings
	{
		float gamma{ 2.2f };
		float exposure{ 1.0f };
	};

	struct ViewWindowResizeEvent : public Event
	{
		ViewWindowResizeEvent(const float2& newSize) : newSize(newSize) {}

		EVENT_CLASS_CATEGORY(EventCategoryPhysics | EventCategoryCollisionDetection);
		EVENT_CLASS_TYPE(EventType::CollisionDetection_EnterSleep)

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "New ViewWindow size (" << newSize.x << ", " << newSize.y << ")";
			return ss.str();
		}

		float2 newSize;
	};

	struct QuadVao
	{
		QuadVao() : vao{ {
			{
				-1, -1, 0,
				+1, -1,	0,
				+1, +1, 0,
				-1, +1,	0,
			},
			{
				0, 0,
				1, 0,
				1, 1,
				0, 1,
			} }, {3, 2}, {0, 1, 2, 0, 2, 3} }
		{
		}


		void draw() const
		{
			vao.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			vao.unbind();
		}

	public:
		VertexArray vao;
	};

	struct CameraMode
	{
		bool fps;
		float lastSwitchedTime;
	};

	struct MouseStatus
	{
		float lastPressedTime;
		float sensibility;
	};

	struct SelfDestructionTimer
	{
		float birth;
		float lifespan;
	};

	class SelfDestruction : public System
	{
	public:
		virtual void update(ArchetypeManager& am, double t, double dt) override
		{
			auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<SelfDestructionTimer>(), {});

			std::vector<Entity> toRemove;
			toRemove.reserve(10);

			for (auto arch : archetypes)
			{
				size_t size = getSize<SelfDestructionTimer>(am, arch);
				auto* selfDestr = get<SelfDestructionTimer>(am, arch);
				auto& entities = getEntities(am, arch);

				for (size_t i = 0; i < size; ++i)
				{
					if (t - selfDestr[i].birth > selfDestr[i].lifespan)
					{
						toRemove.push_back(entities[i]);
					}
				}
			}

			// remove
			ManyEntitiesDestroyedEvent e{ std::move(toRemove) };
			Application::dispatchEventStatic(e);
		}
	};

	class ContactPointEntityRemover : public System
	{
	public:
		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
		{
			auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<ContactEntityTag>(), {});
			for (auto arch : archetypes)
			{
				auto& entities = getEntities(am, arch);
				size_t numEntities = getSize<ContactEntityTag>(am, arch);
				for (size_t i = 0; i < numEntities; ++i)
				{
					Entity e = entities[0];
					Application::getEntityManager()->destroy(e);
					am.removeEntity(e);
				}
			}
		}
	};

	class ContactPointEntityGenerator : public System
	{
	public:
		ContactPointEntityGenerator()
		{
			archetypeCollisionPoint = Archetype{ ComponentList::buildList<CirclePrimitive, Color, Transform, ContactEntityTag>() };
			archetypeCollisionNormal = Archetype{ ComponentList::buildList<SegmentPrimitive, Color, Transform, ContactEntityTag>() };
		}

		void setContacts(const CollisionData* collData) { m_collData = collData; }

		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
		{
			for (auto& arbiter : m_collData->arbiters)
			{
				for (size_t i = 0; i < arbiter.second.manifold.numContacts; ++i)
				{
					const Contact& contact = arbiter.second.manifold.contacts[i];
					createContactEntity(am, arbiter.second.manifold, i);
				}
			}
		}

	private:
		void createContactEntity(r3d::ArchetypeManager& am, const Manifold& manifold, int contactIndex)
		{
			const real3& positionE1 = am.get<Transform>(manifold.e1)->position;
			const rquat& orientationE1 = am.get<Transform>(manifold.e1)->orientation;
			const real3& contactPositionWorld = manifold.contacts[contactIndex].position;

			fquat q{ 1.0, 0.0, 0.0, 0.0 };

			float3 N1{ 1.0f,0.0f,0.0f };
			float3 N2 = (float3)manifold.normal;

			if (glm::length(glm::cross(N2, N1)) > R3D_EPSILON)
			{
				float3 M = glm::normalize(N1 + N2);
				float3 axis = glm::normalize(glm::cross(M, N2));

				float cos = glm::dot(M, N2);
				float sin = glm::length(glm::cross(M, N2));
				q.w = cos;
				q.x = sin * axis.x;
				q.y = sin * axis.y;
				q.z = sin * axis.z;
				q = glm::normalize(q);
			}

			float segLength = 0.333;
			float3 displacement{ 1.0, 0.0, 0.0 };
			float3 rotated_displacement = q * displacement;
			rotated_displacement = segLength * rotated_displacement / glm::length(rotated_displacement);

			auto entity_s = Application::getEntityManager()->create();
			am.setArchetype(entity_s, archetypeCollisionNormal);
			am.set<Color>(entity_s, Color{ float4{1.0, 0.0, 0.0, 1.0} });
			am.set<Transform>(entity_s, Transform{ (float3)contactPositionWorld + rotated_displacement, q, float3{0.3} });

			auto entity_c = Application::getEntityManager()->create();
			am.setArchetype(entity_c, archetypeCollisionPoint);
			am.set<Color>(entity_c, Color{ float4{1.0, 0.0, 0.0, 1.0} });
			am.set<Transform>(entity_s, Transform{ contactPositionWorld, q, float3{0.1} });
		}

	private:
		const CollisionData* m_collData;
		Archetype archetypeCollisionPoint;
		Archetype archetypeCollisionNormal;
	};

	class ShaderEditor;

	class WorldLayer : public r3d::Layer
	{
		// Gameplay
		r3d::SelfDestruction selfDestructionSystem;

		// Physics
		r3d::ForceIntegrator forceIntegrator;
		r3d::VelocityIntegrator velocityIntegrator;
		r3d::SphereSphereContactDetector sphereSphereContactDetector;
		r3d::SpherePlaneContactDetector spherePlaneContactDetector;
		r3d::BoxPlaneContactDetector boxPlaneContactDetector;
		r3d::BoxBoxContactDetector boxBoxContactDetector;
		r3d::ContactPointEntityGenerator contactEntityGenerator;
		r3d::ContactPointEntityRemover contactEntityRemover;
		r3d::CollisionData collisionData;

		// Special entities
		r3d::Entity gun;
		r3d::Entity gridEnt;
		r3d::Entity firstBox;
		r3d::Entity newEntity;
		r3d::float3 newPosition;
		r3d::float3 newAngleAxis;
		r3d::float3 newSize;
		r3d::float3 newColor;
		float newAngle;

		// Demo options
		bool physicsOn;
		bool wireframesOn;
		bool solidsOn;
		bool show_demo_window;

		// Camera and projection
		r3d::Camera camera;
		r3d::float3 deltaPosition;
		r3d::float3 centerPosition;
		r3d::float3 initialPosition;
		r3d::float3 up;

		r3d::float4x4 projectionMatrix;
		float right;
		float top;
		float fovy;
		float aspect;
		float nearPlane;
		float farPlane;
		enum class Projection
		{
			ORTHO,
			PERSP
		} projection;

		// Shaders and renderers
		r3d::Shader linesShader;
		r3d::Shader solidShader;
		r3d::Shader sunLightShader;
		r3d::Shader pointLightShader;
		r3d::Shader hdrShader;
		r3d::Shader blurShader;
		r3d::Shader bloomBrightShader;
		r3d::Shader bloomMixShader;
		r3d::Shader shadowShader;
		r3d::Shader shadowDebugShader;

		r3d::PrimitivesRenderer primitivesRenderer;
		r3d::SolidPrimitivesRenderer solidRenderer;

		// Utils
		int frameCounter;
		MouseStatus mouseStatus;
		CameraMode cameraMode;
		FrameBuffer fboBlur[2];
		FrameBuffer fboBloom;
		FrameBuffer fboBloomMix;
		FrameBuffer fboDefault;
		FrameBuffer fboHDR;
		FrameBuffer fboShadow;
		FrameBuffer fboShadowDebug;

		float2 fboSize;
		float2 fboSizePrev;
		float lastFboResize;
		float2 fboShadowSize;
		float2 viewWindowCenter;
		QuadVao quadVao;
		float lastSpotSwitchedOn;
		
		// shadow settings
		float lighNearPlane{ 2.0f };
		float lighFarPlane{ 50.0f };
		float4 lightLRBT{ -10.0f, 10.0f, -10.0f, 10.0f };
		float minShadowBias{ 0.0001f };
		float maxShadowBias{ 0.001f };


		BloomSettings bloomSettings;
		HDRSettings hdrSettings;

	public:
		WorldLayer();

	private:
		r3d::Entity createSegment(r3d::float3 position, float angleDeg, r3d::float3 axis, float scale, r3d::float4 color);
		template<class Tag> 
		r3d::Entity createBox(r3d::real3 position, r3d::rquat quat, r3d::real3 scale, r3d::real gravity, r3d::real invMass, r3d::real3 velocity, r3d::float4 color, float timer);
		template<class Tag>
		r3d::Entity createBox(r3d::real3 position, r3d::real angle, r3d::real3 axis, r3d::real3 scale, r3d::real gravity, r3d::real invMass = 1.0, r3d::real3 velocity = r3d::real3{ 0.0 }, r3d::float4 color = r3d::float4{ 0.18, 0.4, 0.9, 1.0 }, float timer = -1);
		
		void setUpFbos();

		void setUpFboBloom();
		void setUpFboBlur();
		void setUpFboDefault();
		void setUpFboHDR();
		void setUpFboShadow();
		void setUpFboShadowDebug();

		void editProgram(ShaderEditor* toEdit);

		virtual void onUpdate(r3d::Window* window) override;
		virtual void onRender(r3d::Window* window) override;
		virtual void onImGuiUpdate(r3d::Window* window) override;
		virtual bool onEvent(r3d::Event& e) override;
		bool onMouseButtonPressedEvent(r3d::MouseButtonPressedEvent& e);
		bool onViewWindowResizeEvent(r3d::ViewWindowResizeEvent& e);

	};

}