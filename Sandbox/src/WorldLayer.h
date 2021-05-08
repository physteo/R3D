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

		EVENT_CLASS_CATEGORY(EventCategory::Physics | EventCategory::CollisionDetection);
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

	class WorldLayer : public Layer
	{
		// Gameplay systems
		SelfDestruction selfDestructionSystem;

		// Physics systems
		ForceIntegrator forceIntegrator;
		VelocityIntegrator velocityIntegrator;
		SphereSphereContactDetector sphereSphereContactDetector;
		SpherePlaneContactDetector spherePlaneContactDetector;
		BoxPlaneContactDetector boxPlaneContactDetector;
		BoxBoxContactDetector boxBoxContactDetector;
		ContactPointEntityGenerator contactEntityGenerator;
		ContactPointEntityRemover contactEntityRemover;
		CollisionData collisionData;

		// Rendering systems
		WireframePrimitivesRenderer wireframesRenderer;
		SolidPrimitivesRenderer solidRenderer;

		// Special entities
		Entity gun;
		Entity gridEnt;
		Entity firstBox;
		Entity newEntity;
		float3 newPosition;
		float3 newAngleAxis;
		float3 newSize;
		float3 newColor;
		float newAngle;

		// Demo options
		bool physicsOn;
		bool wireframesOn;
		bool solidsOn;
		bool show_demo_window;

		// Camera and projection
		Camera camera;
		float3 deltaPosition;
		float3 centerPosition;
		float3 initialPosition;
		float3 up;

		float4x4 projectionMatrix;
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

		// Shaders
		Shader solidShader;
		Shader sunLightShader;
		Shader pointLightShader;
		Shader linesShader;
		Shader hdrShader;
		Shader blurShader;
		Shader bloomBrightShader;
		Shader bloomMixShader;
		Shader shadowShader;
		Shader shadowDebugShader;
		Shader skyShader;
		Shader contactsShader;

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
		Entity createSegment(float3 position, float angleDeg, float3 axis, float scale, float4 color);
		template<class Tag> 
		Entity createBox(real3 position, rquat quat, real3 scale, real gravity, real invMass, real3 velocity, float4 color, float timer, const std::string& materialName);
		template<class Tag>
		Entity createBox(real3 position, real angle, real3 axis, real3 scale, real gravity, real invMass = 1.0, real3 velocity = real3{ 0.0 }, float4 color = float4{ 0.18, 0.4, 0.9, 1.0 }, float timer = -1, const std::string& materialName = "");
		
		void setUpFbos();

		void setUpFboBloom();
		void setUpFboBlur();
		void setUpFboDefault();
		void setUpFboHDR();
		void setUpFboShadow();
		void setUpFboShadowDebug();

		void setShadersSettings();

		void editProgram(ShaderEditor* toEdit);

		virtual void onUpdate(Window* window) override;
		virtual void onRender(Window* window) override;
		virtual void onImGuiUpdate(Window* window) override;
		virtual bool onEvent(Event& e) override;
		bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool onViewWindowResizeEvent(ViewWindowResizeEvent& e);

	};

}