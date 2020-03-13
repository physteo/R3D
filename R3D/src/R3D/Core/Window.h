#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <R3D/Core/Core.h>

namespace r3d {

	//! Specify the monitor to use. To be used when creating the Window.
	enum Monitor
	{
		G_PRIMARY = 0,
		G_SECONDARY = 1,
		G_NOTSPECIFIED = -1
	};

	//! Window for rendering.
	class Window
	{
	private:
		GLFWwindow*  m_window;
		double		 m_width;
		double		 m_height;
		GLFWmonitor* m_monitor;
		float        m_fontscale;
		bool		 m_fullscreen;

		float colorR, colorG, colorB;
		float m_lastTime;
		float m_lastFrameTime;
		int   m_maxFramerate;

	public:
		Window() {}
		/*!
		Width and Height must be specified in pixels.
		*/
		Window(std::string title, double width /* in pixels */, double height, int maxFramerate = 60, float fontscale = 1.0);
		/*!
		Width and Height must be specified in pixels. The Monitor can be either G_PRIMARY or G_SECONDARY or G_NOTSPECIFIED. G_NOTSPECIFIED will take the default primary
		monitor. The scaling of fonts can also be specified.
		*/
		Window(std::string title, double width /* in pixels */, double height, Monitor monitor, int maxFramerate = 60, float fontscale = 1.0);

		bool isClosed() const;
		void close() const { glfwSetWindowShouldClose(m_window, true); }
		void setViewPort(float width, float height) const;
		//!< Set the "background color" of the currently binded buffer. Use numbers between 0 and 1 (not 0-255!).
		void clearColorBufferBit(float red, float blue, float green, float alpha) const;
		void clearColorBufferBit() const;
		void swapBuffers() const;
		void pollEvents() const;
		void terminate() const;

		void setWidth(double width) { m_width = width; }
		void setHeight(double height) { m_height = height; }
		void setColor(float r, float g, float b) { colorR = r; colorG = g; colorB = b; }
		void setCursorDisabled() const;
		void setCursorVisible() const;
		void setCursorPosition(std::pair<float, float> pos) const;
		void setFontscale(float fontscale);
		double getWidth() const { return m_width; }
		double getHeight() const { return m_height; }
		float getColorR() const { return colorR; }
		float getColorG() const { return colorG; }
		float getColorB() const { return colorB; }
		float getFontscale() const { return m_fontscale; }
		//!< Updates the stored time (in seconds) with the real time.
		void  updateTime();
		//!< Gets the latest stored time (stored when using updateTime)
		double getLastTime() const;
		//!< Update the time it took to process this frame (in seconds). To be used at the end of each frame.
		void  updateLastFrameTime();
		//!< Gets the time it took to process the previous frame (in seconds). 
		double getLastFrameTime() const;
		//!< Gets the real time, measured from the start of the program (in seconds) 
		double getCurrentTime() const;
		//!< Gets the maximum number of frames per second.
		inline int getMaxframerate() const { return m_maxFramerate; }

		const GLFWwindow* getGLFWwindow() const;
		GLFWwindow*		  getGLFWwindow();
	};

}