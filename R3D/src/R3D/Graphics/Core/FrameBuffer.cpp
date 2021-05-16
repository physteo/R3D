#include <R3Dpch.h>
#include "FrameBuffer.h"

namespace r3d
{

	FrameBuffer::FrameBuffer()
	{
		generate();
	}

	FrameBuffer::~FrameBuffer()
	{
		release();
	}

	FrameBuffer::FrameBuffer(FrameBuffer&& other)
	{
		swapData(other);
	}

	FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other)
	{
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	void FrameBuffer::generate()
	{
		glGenFramebuffers(1, &m_id); // TODO: why glCREATEFrameBuffers was causing crashes on a different machine?
		m_currentTarget = GL_FRAMEBUFFER;
	}


	void FrameBuffer::bind(GLenum target)
	{
		m_currentTarget = target;
		glBindFramebuffer(target, m_id);
	}

	void FrameBuffer::unbind()
	{
		glBindFramebuffer(m_currentTarget, 0);
	}

	void FrameBuffer::attach2DTexture(GLenum attachment, int width, int height, TextureFormat internalFormat,
		TextureFormat format, GLenum dataFormat, GLint filter, GLint wrap)
	{
		// create texture
		unsigned int textureID = 0;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		if (wrap == GL_CLAMP_TO_BORDER)
		{
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataFormat, nullptr);

		bind();
		glFramebufferTexture(m_currentTarget, attachment, textureID, 0);
		unbind();

		//drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + int(textureAttachments.size()) );
		drawBuffers.push_back(attachment);

		// unbind texture and store id
		glBindTexture(GL_TEXTURE_2D, 0);
		textureAttachments.push_back(textureID);
	}

	void FrameBuffer::attachRenderBuffer(GLenum internalFormat, int width, int height)
	{
		unsigned int renderBufferID = 0;
		glGenRenderbuffers(1, &renderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

		// attach texture and renderbuffer to framebuffer
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
		unbind();

		// unbind renderframebuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		renderBuffersAttachments.push_back(renderBufferID);
	}

	void FrameBuffer::setDrawBuffers(bool set)
	{
		bind();
		if (set)
		{
			glDrawBuffers((GLsizei)drawBuffers.size(), &drawBuffers[0]);
		}
		else
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		unbind();
	}

	bool FrameBuffer::isComplete()
	{
		bind();
		// TODO: allow return value of GLCall so that I can use it here.
		bool result = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		unbind();
		return result;
	}


	void FrameBuffer::release()
	{
		// release RenderBuffers
		for (auto it = renderBuffersAttachments.begin(); it != renderBuffersAttachments.end(); ++it)
		{
			glDeleteRenderbuffers(1, &(*it));
		}
		renderBuffersAttachments.clear();

		for (auto it = textureAttachments.begin(); it != textureAttachments.end(); ++it)
		{
			glDeleteTextures(1, &(*it));
		}
		textureAttachments.clear();

		drawBuffers.clear();

		// release FrameBuffer
		glDeleteFramebuffers(1, &m_id);
		m_id = 0;
	}

	void FrameBuffer::swapData(FrameBuffer& other)
	{
		// move
		m_id = other.m_id;
		m_currentTarget = other.m_currentTarget;
		for (auto otherIt = other.textureAttachments.begin(); otherIt != other.textureAttachments.end(); ++otherIt)
		{
			textureAttachments.emplace_back(*otherIt);
			*otherIt = 0;
		}

		for (auto otherIt = other.renderBuffersAttachments.begin(); otherIt != other.renderBuffersAttachments.end(); ++otherIt)
		{
			renderBuffersAttachments.push_back(*otherIt);
			*otherIt = 0;
		}

		for (auto otherIt = other.drawBuffers.begin(); otherIt != other.drawBuffers.end(); ++otherIt)
		{
			drawBuffers.push_back(*otherIt);
			*otherIt = 0;
		}

		other.m_id = 0;
		// other.m_currentTarget can stay the same

	}

}