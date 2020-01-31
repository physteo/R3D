#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace r3d
{

	enum BufferAccess
	{
		READ = GL_READ_ONLY,
		WRITE = GL_WRITE_ONLY,
		READ_WRITE = GL_READ_WRITE
	};


	//! Wrapper of the OpenGL vertex buffer object.
	/*!
		Send buffer to the GPU, keeping a handle (m_id) and its type (m_type) which
		can be GL_ELEMENT_ARRAY_BUFFER for index buffer objects or GL_ARRAY_BUFFER for vertex
		buffer objects.
		IMPORTANT: while constructing the Vertex array object (Vao), you need to keep
		the Buffer bind, when using glEnableVertexAttribArray and glVertexAttribPointer.
		Also, if using it as index buffer object, you need to unbind it only after unbinding the Vao.
	*/
	class Buffer
	{
	private:
		unsigned int m_id;
		unsigned int m_type;
	public:
		Buffer() : m_id(0), m_type(GL_ARRAY_BUFFER) {}
		Buffer(unsigned int type, const void* data, size_t size);
		Buffer(const void* data, size_t size, size_t binding);

		~Buffer();

		// Cannot use the copy constructor/assignment.
		Buffer(const Buffer &) = delete;
		Buffer& operator=(const Buffer &) = delete;

		// Can use move constructor/assignment.
		Buffer(Buffer&& other);
		Buffer& operator=(Buffer&& other);

		void bind()   const;
		void unbind() const;

		void generate();
		void setData(const void* data, size_t size);

		void* map(BufferAccess access);
		void  unmap();

	private:
		void bind(unsigned int id)   const;

		void swapData(Buffer& other);
		void release();
	};

}

// GLCall\((.+)\)
// $1