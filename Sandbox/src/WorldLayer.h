#pragma once
#include <R3D.h>

#include <R3D/Utils/Debugger.h>

#include <R3D/Physics/Core/Colliders.h>
#include <R3D/Physics/Core/Topology.h>
#include <R3D/Physics/Core/World.h>
#include <R3D/Physics/Systems/Integrator.h>

#include <R3D/Graphics/Core/Palette.h>
#include <R3D/Graphics/Systems/SolidPrimitivesRenderer.h>

#include "ContactEntityGenerator.h"

#define STACKING

namespace r3d
{

	const float stdGravity{-10.0f};

	struct WallTag {};
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

	struct CameraMode
	{
		bool fps{ false };
		float lastSwitchedTime{ 0.0f };
	};

	struct MouseStatus
	{
		float lastPressedTime{ 0.0f };
		float sensibility{ 0.05f };
	};

	struct SelfDestructionTimer
	{
		float birth;
		float lifespan;
	};

	class SelfDestruction : public System
	{
	public:
		virtual void update(ArchetypeManager& am, double t, double dt) override;
	};

	class ShaderEditor;

	class WorldLayer : public r3d::Layer
	{
		// Gameplay systems
		r3d::SelfDestruction selfDestructionSystem;

		// Physics systems
		r3d::ForceIntegrator forceIntegrator;
		r3d::VelocityIntegrator velocityIntegrator;
		r3d::SphereSphereContactDetector sphereSphereContactDetector;
		r3d::SpherePlaneContactDetector spherePlaneContactDetector;
		r3d::BoxPlaneContactDetector boxPlaneContactDetector;
		r3d::BoxBoxContactDetector boxBoxContactDetector;
		r3d::ContactPointEntityGenerator contactEntityGenerator;
		r3d::ContactPointEntityRemover contactEntityRemover;
		r3d::CollisionData collisionData;

		// Rendering systems
		r3d::WireframePrimitivesRenderer wireframesRenderer;
		r3d::SolidPrimitivesRenderer solidRenderer;

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
		enum class ProjectionType
		{
			ORTHO,
			PERSP
		} projectionType;

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
		r3d::Shader skyShader;
		r3d::Shader contactsShader;

		// Framebuffers
		FrameBuffer fboBlur[2];
		FrameBuffer fboBloom;
		FrameBuffer fboBloomMix;
		FrameBuffer fboDefault;
		FrameBuffer fboHDR;
		FrameBuffer fboShadow;
		FrameBuffer fboShadowDebug;
		float2 fboSize;
		float2 fboSizePrev;
		float lastFboResize{ 0.0f };
		float2 fboShadowSize{ 1024, 1024 };

		// shadow settings
		float lighNearPlane{ 2.0f };
		float lighFarPlane{ 50.0f };
		float4 lightLRBT{ -13.0f, 13.0f, -13.0f, 13.0f };
		float minShadowBias{ 0.0001f };
		float maxShadowBias{ 0.001f };

		// Utils
		int frameCounter{ 0 };
		MouseStatus mouseStatus;
		CameraMode cameraMode;
		BloomSettings bloomSettings;
		HDRSettings hdrSettings;
		float2 viewWindowCenter{ 0.0f, 0.0f };
		float lastSpotSwitchedOn{ 0.0f };

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