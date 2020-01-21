#include <R3D.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

struct Position
{
	Position(float x = 0.0f) : x(x) {}
	float x;
};

class UpdatePositionSys : public r3d::System
{

public:
	UpdatePositionSys() : System() {}
	virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
	{
		auto archetypes = am.matchAtLeastWithout(r3d::ComponentList::buildList<Position>(), {});
		for (auto arch = archetypes.begin(); arch != archetypes.end(); ++arch)
		{
			auto numEnts = getSize<Position>(am, *arch);
			auto positions = get<Position>(am, *arch);
			for (size_t i = 0; i < numEnts; ++i)
			{
				positions[i].x = sin(t);
			}
		}
	}
};

class WorldLayer : public r3d::Layer
{
	UpdatePositionSys updatePositionSys;
	r3d::Entity lel;
	bool show_demo_window, show_another_window;
public:
	WorldLayer() : r3d::Layer("World layer.")
	{
		lel = r3d::Application::getEntityManager()->create();
		auto am = getArchetypeManager();
		am->setArchetype<Position>(lel);

		show_demo_window = true;
		show_another_window = true;
	}

	virtual void onUpdate(r3d::Window* window) override
	{
		double t = window->getLastTime();
		double dt = window->getLastFrameTime();
		auto am = getArchetypeManager();
		updatePositionSys.update(*am, t, dt);
	}

	virtual void onRender(r3d::Window* window) override
	{
	}

	virtual void onImGuiUpdate(r3d::Window* window) override
	{
		auto am = getArchetypeManager();
		static float f = 0.0f;
		static int counter = 0;
		float clear_color[3];
		clear_color[0] = window->getColorR();
		clear_color[1] = window->getColorG();
		clear_color[2] = window->getColorB();

		show_demo_window = true;
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);
		
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		window->setColor(clear_color[0], clear_color[1], clear_color[2]);

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::Text("Position = %f", am->get<Position>(lel).x);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	virtual bool onEvent(r3d::Event& e) override
	{
		return false;
	}

};

class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{ "Sandbox app.", 800, 600 }
	{
		setDebugMode(false);

		pushBackLayer(new WorldLayer());
	}
	~Sandbox() {}
};

r3d::Application* r3d::createApplication()
{
	return new Sandbox();
}

/*

	virtual void run() override
	{

		r3d::Window window{ "Sandbox app.", 1000, 800, 60 };

		// entities and stuff
		auto lel = entityMgr.create();
		archetypeMgr.setArchetype<Position>(lel);
		archetypeMgr.set<Position>(lel, Position{ 0.0 });

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
			window.clearColorBufferBit(clear_color[0], clear_color[1], clear_color[2], 1.0f);
			double t = window.getLastTime();
			double dt = window.getLastFrameTime();

			// ******* logic
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
			
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);
				ImGui::Text("Position = %f", archetypeMgr.get<Position>(lel).x);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			// imgui render
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// ecs stuff
			updatePositionSys.update(archetypeMgr, t, dt);

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
*/

 
