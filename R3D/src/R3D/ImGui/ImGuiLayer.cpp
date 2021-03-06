#include <R3Dpch.h>

#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include "../Core/Window.h"

namespace r3d
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::onUpdate(Window* window)
	{
		// Start the Dear ImGui frame 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::onRender(Window* window)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool ImGuiLayer::onEvent(Event &e)
	{
		return false;
	}

	void ImGuiLayer::init(Window* window)
	{
		// Init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplOpenGL3_Init("#version 410");
		ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
		ImGui::GetStyle().ScaleAllSizes(window->getFontscale());
	}

	void ImGuiLayer::shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

}

namespace ImGui
{

	bool Begin(const char* name, float fontscale, bool* p_open, ImGuiWindowFlags flags)
	{
		bool result = ImGui::Begin(name, p_open, flags);
		ImGui::SetWindowFontScale(fontscale);
		return result;
	}

}
