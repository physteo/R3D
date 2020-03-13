#pragma once

#include <R3D/Core/Layer.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

namespace r3d
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		void init(Window* window);
		void shutdown();
		virtual void onUpdate(Window* window) override;
		virtual void onRender(Window* window) override;
		virtual bool onEvent(Event& e) override;
	};

}

namespace ImGui
{

	IMGUI_API bool Begin(const char* name, float fontscale, bool* p_open = NULL, ImGuiWindowFlags flags = 0);

}