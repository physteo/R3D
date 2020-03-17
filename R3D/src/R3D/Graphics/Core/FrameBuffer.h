#pragma once

#include <glad/glad.h>
#include<GLFW/glfw3.h>

namespace r3d
{

	enum TextureFormat
	{
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		sRGB = GL_SRGB,
		sRGBA = GL_SRGB_ALPHA,
		RGBA16 = GL_RGBA16,
		RGBA16F = GL_RGBA16F,
		DEPTH = GL_DEPTH_COMPONENT
	};

	//! Class for OpenGL Framebuffers
	/*!
		Stores a FrameBuffer with its ID.
		After creation, one needs to attach a to make it work.
		Textures and RenderBuffers can be attached, and are owned by the FrameBuffer object.
	*/
	class FrameBuffer
	{
	public:
		FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;

		FrameBuffer(FrameBuffer&& other);
		FrameBuffer& operator=(FrameBuffer&& other);

		~FrameBuffer();

		inline unsigned int getTextureID(size_t i) { return textureAttachments[i]; }

		//!< Binds the target to this framebuffer. Default target is GL_FRAMEBUFFER. It sets an internal state that keeps track of current target.  
		void bind(GLenum target = GL_FRAMEBUFFER);
		//!< Binds the current framebuffer target to zero.
		void unbind();
		//!< creates a texture and owns it, stores its ID.
		void attach2DTexture(GLenum attachment, int width, int height, TextureFormat internalFormat, 
			TextureFormat format, GLenum dataFormat, GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);
		////!< Attaches a texture, taking its ownership.
		//void attach2DTexture(GLenum attachment, Texture&& texture);
		//!< creates a renderbuffer and owns it, stores its ID.
		void attachRenderBuffer(GLenum internalFormat, int width, int height);

		void setDrawBuffers(bool set = true);

		//!< check if the framebuffer is complete (OpenGL requires at least one color attachment).
		bool isComplete();
		////!< retrieves the ID of the specified texture.
		//inline unsigned int getAttachedTextureID(size_t i) const { return textureAttachments.at(i).getID(); }

	private:
		unsigned int m_id;
		GLenum  m_currentTarget;

		std::vector<unsigned int> textureAttachments;
		std::vector<unsigned int> renderBuffersAttachments;
		std::vector<GLenum> drawBuffers;
		void generate();

		void release();
		void swapData(FrameBuffer& other);
	};

}
