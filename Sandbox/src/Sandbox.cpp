#include <R3D.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>


class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{} {}
	~Sandbox() {}

	virtual void run() override
	{

		r3d::Window window{ "Sandbox app.", 1000, 800, 60 };

		// imgui
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);

		// stuff to modify
		bool show_demo_window = true;
		auto show_another_window = true;
		float clear_color[3];
		clear_color[0] = 1.0f;
		clear_color[1] = 0.0f;
		clear_color[2] = 0.0f;


		// Game loop
		while (!window.isClosed())
		{
			// ******* initialize frame
			window.updateTime();
			window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

			// ******* logic
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// imgui render
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			// ******* last stuff to do
			window.swapBuffers();
			window.updateLastFrameTime();

			// **********  fix maximum framerate
			int frameTimeMicrosec = (int)(window.getLastFrameTime() * 1000000.0f);
			int waitingTime = (int)(1000000.0f / window.getMaxframerate()) - frameTimeMicrosec;
			if (waitingTime > 0)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(waitingTime));
				window.updateLastFrameTime();
			}

			window.pollEvents();
		}

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

	}
};

r3d::Application* r3d::createApplication()
{
	r3d::EventManager eventMgr;
	r3d::EntityManager entityMgr;
	r3d::ArchetypeManager archetypeMgr;
	
	Entity a;
	
	r3d::Entity e = entityMgr.create();
	archetypeMgr.setArchetype<int>(e);

	return new Sandbox();
}
 
