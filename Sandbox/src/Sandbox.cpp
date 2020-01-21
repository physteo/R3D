#include <R3D.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;


class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{} {}
	~Sandbox() {}

	virtual void run() override
	{

		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
		glfwMakeContextCurrent(window);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}

		// Set the required callback functions
		glfwSetKeyCallback(window, key_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			exit(-1);
		}

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
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings

		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui_ImplGlfw_InitForOpenGL(window, true);

		// Define the viewport dimensions
		glViewport(0, 0, WIDTH, HEIGHT);

		// stuff to modify
		bool show_demo_window = true;
		auto show_another_window = true;
		float clear_color[3];
		clear_color[0] = 1.0f;
		clear_color[1] = 0.0f;
		clear_color[2] = 0.0f;


		// Game loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();


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

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			// Render
			// Clear the colorbuffer
			glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// imgui render
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			// Swap the screen buffers
			glfwSwapBuffers(window);
		}


		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// Terminates GLFW, clearing any resources allocated by GLFW.
		glfwTerminate();
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
 

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}