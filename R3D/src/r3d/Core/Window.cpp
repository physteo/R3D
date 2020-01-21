#include <R3Dpch.h>

#include "Window.h"
#include <R3D/Core/Log.h>

namespace r3d
{

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         R3D_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       R3D_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          R3D_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: R3D_CORE_TRACE(message); return;
		}

		R3D_CORE_ASSERT(false, "Unknown severity level!");
	}

#endif

	Window::Window(std::string title, double width, double height, int maxFramerate) : Window{ title, width, height, Monitor::G_NOTSPECIFIED, maxFramerate } {}

	Window::Window(std::string title, double width, double height, Monitor monitor, int maxFramerate)
	{
		m_lastTime = getCurrentTime();
		R3D_ASSERT(maxFramerate > 0, "MaxFramerate must be positive.");
		m_maxFramerate = maxFramerate;
		m_lastFrameTime = (float)(1.0 / ((double)maxFramerate));

		m_width = width;
		m_height = height;
		
		// initialize the glfw library 
		if (!glfwInit()) {
			R3D_CORE_CRITICAL("Failed to initialize GLFW.");
			throw -1;
		}

		// get primary monitor
		int numberOfMonitors;
		GLFWmonitor** monitors = glfwGetMonitors(&numberOfMonitors);

		// specify if core or compatibility mode 
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// GLFW_OPENGL_CORE_PROFILE); 
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		// keep the size fixed 
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // TODO: make windows resizable

		// Create a windowed mode window and its OpenGL context 
		if (monitor == Monitor::G_NOTSPECIFIED)
		{
			m_window = glfwCreateWindow((int) width, (int) height, title.c_str(), nullptr, nullptr);
		}
		else
		{
			m_window = glfwCreateWindow((int) width, (int) height, title.c_str(), monitors[monitor], nullptr);
		}

		if (!m_window)
		{
			R3D_CORE_CRITICAL("Failed to create GLFWwindow. Terminating GLFW.");
			glfwTerminate();
			throw -1;
		}

		// Make the window's current context 
		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			R3D_CORE_CRITICAL("Failed to initialize OpenGL context.");
			glfwTerminate();
			throw -1;
		}

		// configure global opengl state
		glEnable(GL_DEPTH_TEST);
		//stencil//glDepthFunc(GL_LESS);
		//stencil//glEnable(GL_STENCIL_TEST);
		//stencil//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//stencil//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CCW);

		// To have hdr: comment this, and in the hdr shader do abilitate the calculations for the hdr
		//glEnable(GL_FRAMEBUFFER_SRGB);

		// set input mode for mouse
		glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, 1);

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
#endif
		// set the viewport
		setViewPort(m_width, m_height);

		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		R3D_CORE_INFO("OpenGL: version {0}.{1}", versionMajor, versionMinor);
		R3D_CORE_INFO("GLFW:   version {0}", glfwGetVersionString());
		
		R3D_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "R3D requires at least OpenGL version 4.5!");
	}

	void Window::setViewPort(float width, float height) const
	{
		glViewport(0, 0, width, height);
	}

	bool Window::isClosed() const
	{
		return glfwWindowShouldClose(m_window);
	}

	void Window::clearColorBufferBit(float red, float green, float blue, float alpha) const
	{
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//stencil//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	}

	void Window::swapBuffers() const
	{
		glfwSwapBuffers(m_window);
	}

	void Window::pollEvents() const
	{
		glfwPollEvents();
	}

	void Window::terminate() const
	{
		glfwTerminate();
	}

	double Window::getLastTime() const
	{
		return m_lastTime;
	}

	double Window::getLastFrameTime() const
	{
#ifdef R3D_FIXED_FRAMERATE
		return 1.0 / ((double)R3D_FIXED_FRAMERATE);
#else
		return m_lastFrameTime;
#endif
	}

	double Window::getCurrentTime() const
	{
		return glfwGetTime();
	}

	void  Window::updateTime()
	{
		m_lastTime = getCurrentTime();
	}

	void Window::updateLastFrameTime()
	{
		m_lastFrameTime = getCurrentTime() - m_lastTime;
	}

	const GLFWwindow* Window::getGLFWwindow() const
	{
		return m_window;
	}

	GLFWwindow* Window::getGLFWwindow()
	{
		return m_window;
	}
}