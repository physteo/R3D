#pragma once

#include <R3D/Core/Layer.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

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