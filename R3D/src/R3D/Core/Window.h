#pragma once

#include <R3D/Core/Core.h>

class GLFWwindow;

namespace r3d {

	class Window final
	{
	public:
		Window(std::string title, double width, double height, int maxFramerate = 60, float fontscale = 1.0);

		bool isClosed() const;
		void close() const;
		void setViewPort(float width, float height) const;
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

		double getWidth()     const { return m_width; }
		double getHeight()    const { return m_height; }
		float  getColorR()    const { return colorR; }
		float  getColorG()    const { return colorG; }
		float  getColorB()    const { return colorB; }
		float  getFontscale() const { return m_fontscale; }
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

	private:
		GLFWwindow*  m_window;
		double		 m_width;
		double		 m_height;
		float        m_fontscale;

		float colorR{ 0.05f }, colorG{ 0.05f }, colorB{ 0.05f };
		float m_lastTime;
		float m_lastFrameTime;
		int   m_maxFramerate;

	};

}