#include <R3Dpch.h>
#include "Buffer.h"

namespace r3d
{

	Buffer::Buffer(unsigned int type, const void* data, size_t size) : m_type(type)
	{
		generate();
		setData(data, size);
	}

	Buffer::Buffer(const void* data, size_t size, size_t binding) : m_type(GL_SHADER_STORAGE_BUFFER)
	{
		generate();
		setData(data, size);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)binding, (GLuint)m_id);
	}

	Buffer::~Buffer()
	{
		release();
	}

	Buffer::Buffer(Buffer&& other)
	{
		swapData(other);
	}

	Buffer& Buffer::operator=(Buffer&& other)
	{
		// check for self-assignment.
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	void Buffer::bind()   const
	{
		bind(m_id);
	}
	void Buffer::bind(unsigned int id)   const
	{
		glBindBuffer(m_type, id);
	}


	void Buffer::unbind() const
	{
		bind(0);
	}


	void* Buffer::map(BufferAccess access)
	{
		void* data = nullptr;
		data = glMapBuffer(m_type, access);
		return data;
	}

	void Buffer::unmap()
	{
		glUnmapBuffer(m_type);
	}

	void Buffer::generate() {
		glGenBuffers(1, &m_id);
	}

	void Buffer::setData(const void* data, size_t size)
	{
		// what was bound before?
		//GLuint boundBuffer = 0;
		//glGetIntegerv(m_type, (GLint*) &boundBuffer);

		// bind
		bind();
		glBufferData(m_type, size, data, GL_STATIC_DRAW);

		// bind what was bind before
		//bind(boundBuffer); // <- this is a mistake: need to keep the buffer binded while constructing vao
	}

	void Buffer::swapData(Buffer& other)
	{
		m_id = other.m_id;
		m_type = other.m_type;

		other.m_id = 0;
		// other.m_type remains the same
	}

	void Buffer::release()
	{
		glDeleteBuffers(1, &m_id);
		m_id = 0;
	}

}