#pragma once

// For use by R3D applications

#include <R3D/Core/Log.h>
#include <R3D/Core/MathUtils.h>
#include <R3D/Core/Application.h>
#include <R3D/Core/Window.h>
#include <R3D/Core/Input.h>
#include <R3D/Utils/Random.h>
#include <R3D/Events/EventManager.h>
#include <R3D/Ecs/EntityManager.h>
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/Ecs/System.h>
#include <R3D/Ecs/TransformComponent.h>
#include <R3D/ImGui/ImGuiCustom.h>

#include <R3D/Graphics/Core/Camera.h>
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Core/FrameBuffer.h>
#include <R3D/Graphics/Core/Material.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Graphics/Systems/WireframePrimitivesRenderer.h>
#include <R3D/Graphics/Systems/SolidPrimitivesRenderer.h>

#include <R3D/Physics/Core/World.h>
#include <R3D/Physics/Core/Contact.h>
#include <R3D/Physics/Core/RigidBody.h>
#include <R3D/Physics/Systems/SphereSphereContactDetector.h>
#include <R3D/Physics/Systems/SpherePlaneContactDetector.h>
#include <R3D/Physics/Systems/BoxPlaneContactDetector.h>
#include <R3D/Physics/Systems/BoxBoxContactDetector.h>
