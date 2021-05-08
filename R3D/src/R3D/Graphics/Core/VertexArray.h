#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Buffer.h"

namespace r3d
{

	//!< Wrapper of the OpenGL Vertex Array Object (VAO).
	/*!
		Stores what is needed to draw vertices. Includes the format of the data, and the data (as Vertex Buffer
		Objects), which is owned by the VAO object (but it does not copy such data, only has reference to it).
		Stores the handle of the data m_id and the m_ibo / m_vbo.
		IMPORTANT: The default object (m_id = 0) is not accepted while in CORE profile.
	*/
	class VertexArray
	{
	private:
		Buffer m_vbo;
		Buffer m_ibo;
		unsigned int m_id;
		unsigned int m_numIndices;

	public:
		VertexArray() : m_vbo{}, m_ibo{}, m_id(0), m_numIndices(0) {}
		VertexArray(const std::vector<std::vector<float> >& attributes, const std::vector<unsigned int>& components, const std::vector<unsigned int>& indices);

		~VertexArray();

		VertexArray(const VertexArray& other) = delete;
		VertexArray& operator=(const VertexArray& other) = delete;

		VertexArray(VertexArray&& other);
		VertexArray& operator=(VertexArray&& other);

		void bind()   const { glBindVertexArray(m_id); }
		void unbind() const { glBindVertexArray(0); }

		void draw(GLenum mode) const;

		inline unsigned int numIndices() const { return m_numIndices; }
	private:
		void generate() { glGenVertexArrays(1, &m_id); }
		void fillData(const std::vector<std::vector<float> >& attributes, const std::vector<unsigned int>& components);
		void setIbo(const std::vector<unsigned int>& indices);

		void swapData(VertexArray& other);
		void release();
	};

	class Vaos
	{

	public:
		static Vaos& getInstance();
		static VertexArray createCubeVNTUMesh();
		static VertexArray createSphereMesh();
		static VertexArray createSkyDomeMesh(float radius);

		const VertexArray point;
		const VertexArray quad;
		const VertexArray plane;
		const VertexArray cube;
		const VertexArray cubeVNTU;
		const VertexArray cubeWithUv;
		const VertexArray skyDome;
		const VertexArray circle;

	private:
		Vaos();

	};

}

