#include <R3Dpch.h>
#include "../../Core/Log.h"
#include "VertexArray.h"

namespace r3d
{

	VertexArray::VertexArray(const std::vector<std::vector<float> >& attributes, const std::vector<unsigned int>& components, const std::vector<unsigned int>& indices)
	{
		generate();
		bind();
		fillData(attributes, components);
		setIbo(indices);
		unbind();
	}

	VertexArray::~VertexArray()
	{
		release();
	}

	VertexArray::VertexArray(VertexArray&& other)
	{
		swapData(other);
	}

	VertexArray& VertexArray::operator=(VertexArray&& other)
	{
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	void VertexArray::fillData(const std::vector<std::vector<float>>& attributes, const std::vector<unsigned int>& components)
	{
		std::vector<float> data;
		if (components.size() == 0 || attributes.size() == 0)
		{
			R3D_CORE_WARN("Data is empty. Cannot fill VertexArray.");
			return;
		}

		unsigned int numVertices = (unsigned int)attributes.at(0).size() / components.at(0);

		/* put data in format VNTVNTVNTVNT */
		for (size_t i = 0; i < numVertices; i++)
		{
			for (size_t j = 0; j < attributes.size(); j++)
			{
				for (size_t k = 0; k < components.at(j); k++)
				{
					data.push_back(attributes.at(j).at(k + i * components.at(j)));
				}
			}
		}

		/* number of floats for each vertex */
		int stride = 0;
		for (size_t i = 0; i < components.size(); i++)
		{
			stride += components.at(i);
		}

		/* put data in the GPU */
		Buffer vbo{ GL_ARRAY_BUFFER, &data[0], sizeof(float) * data.size() };
		m_vbo = std::move(vbo);

		for (size_t i = 0; i < attributes.size(); i++)
		{
			// compute the stride for this attribute
			int offset = 0;
			for (size_t j = 0; j < i; j++)
			{
				offset += components.at(j);
			}

			glEnableVertexAttribArray((GLuint)i);
			glVertexAttribPointer((GLuint)i, (GLint)components.at(i), GL_FLOAT, GL_FALSE, (GLsizei)stride * sizeof(float), (void*)(offset * sizeof(float)));
		}

		m_vbo.unbind(); // TODO: necessary?
		unbind();
	}

	void VertexArray::setIbo(const std::vector<unsigned int>& indices)
	{
		bind();

		Buffer ibo{ GL_ELEMENT_ARRAY_BUFFER, &indices[0], indices.size() * sizeof(unsigned int) };
		m_ibo = std::move(ibo);

		unbind();
		m_ibo.unbind(); // Note: need to first unbind the vao.
		
		m_numIndices = (unsigned int)indices.size();
	}

	void VertexArray::swapData(VertexArray& other)
	{
		m_id = other.m_id;
		other.m_id = 0;
		m_ibo = std::move(other.m_ibo);
		m_vbo = std::move(other.m_vbo);
		m_numIndices = other.m_numIndices;
		other.m_numIndices = 0;
	}

	void VertexArray::release()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void VertexArray::draw(GLenum mode) const
	{
		glDrawElements(mode, m_numIndices, GL_UNSIGNED_INT, 0);
	}

	static std::vector<unsigned int> quadIndices{ 0, 1, 2, 0, 2, 3 };

	static std::vector<float> quadVertices =
	{
		-1, -1, 0,
		+1, -1,	0,
		+1, +1, 0,
		-1, +1,	0,
	};

	static std::vector<float> quadUVs =
	{
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	static std::vector<unsigned int> quadComponents{ 3, 2 };

	static std::vector<unsigned int> planeIndices = { 0, 1, 2, 0, 2, 3 };

	static std::vector<float> planeVertices = {
					-1, -1, 0,
					-1, +1,	0,
					+1, +1, 0,
					+1, -1,	0,
	};

	static std::vector<float> planeNormals = {
					0, 0, 1,
					0, 0, 1,
					0, 0, 1,
					0, 0, 1,
	};

	static std::vector<float> planeTangents = {
					1, 0, 0,
					1, 0, 0,
					1, 0, 0,
					1, 0, 0,
	};

	static std::vector<unsigned int> planeComponents{ 3, 3, 3 };

	static std::vector<unsigned int> cubeIndices =
	{
		0, 1, 2, 0, 2, 3,       // front
		4, 5, 6, 4, 6, 7,       // right
		8, 9, 10, 8, 10, 11,    // back
		12, 13, 14, 12, 14, 15, // left
		16, 17, 18, 16, 18, 19, // up
		20, 21, 22, 20, 22, 23  // down
	};

	static std::vector<float> cubeVertices =
	{
		// front                        
		-0.5, -0.5, +0.5,  // 0     
		+0.5, -0.5, +0.5,	// 1	
		+0.5, +0.5, +0.5,	// 2	
		-0.5, +0.5, +0.5,	// 3	
		// right					
		+0.5, -0.5, +0.5,	// 4	
		+0.5, -0.5, -0.5,	// 5	
		+0.5, +0.5, -0.5,	// 6	
		+0.5, +0.5, +0.5,  // 7		
		// back						
		-0.5, -0.5, -0.5,  // 8		
		-0.5, +0.5, -0.5,	// 9	
		+0.5, +0.5, -0.5,	// 10	
		+0.5, -0.5, -0.5,	// 11	
		// left						
		-0.5, -0.5, -0.5, // 12		
		-0.5, -0.5, +0.5, // 13		
		-0.5, +0.5, +0.5, // 14		
		-0.5, +0.5, -0.5, // 15		
		// up						
		-0.5, +0.5, +0.5, // 16		
		+0.5, +0.5, +0.5, // 17		
		+0.5, +0.5, -0.5, // 18		
		-0.5, +0.5, -0.5, // 19		
		// down						
		-0.5, -0.5, -0.5, // 20		
		+0.5, -0.5, -0.5, // 21		
		+0.5, -0.5, +0.5, // 22		
		-0.5, -0.5, +0.5  // 23		
	};

	static std::vector<float> cubeNormals =
	{
		// front
		0., 0., 1.,
		0., 0., 1.,
		0., 0., 1.,
		0., 0., 1.,
		// right
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		// back
		0., 0., -1.,
		0., 0., -1.,
		0., 0., -1.,
		0., 0., -1.,
		// left
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,
		// up
		0., 1., 0.,
		0., 1., 0.,
		0., 1., 0.,
		0., 1., 0.,
		// down
		0., -1., 0.,
		0., -1., 0.,
		0., -1., 0.,
		0., -1., 0.
	};

	static std::vector<float> cubeTangents =
	{
		// front
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		// right
		0., 0., -1.,
		0., 0., -1.,
		0., 0., -1.,
		0., 0., -1.,
		// back
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,
		// left
		0., 0., 1.,
		0., 0., 1.,
		0., 0., 1.,
		0., 0., 1.,
		// up
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,
		// down
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.
	};

	static std::vector<float> cubeUVs = 
	{
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// right
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0, 
		// back
		0.0, 0.0, 
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		// left
		0.0, 0.0, 
		0.0, 1.0, 
		1.0, 1.0, 
		1.0, 0.0, 
		// up
		0.0, 1.0, 
		1.0, 1.0, 
		1.0, 0.0, 
		0.0, 0.0, 
		// down
		0.0, 0.0, 
		1.0, 0.0, 
		1.0, 1.0, 
		0.0, 1.0  
	};

	static std::vector<unsigned int> cubeComponents{ 3, 3, 3 };

	static VertexArray create_dome_mesh(float radius);
	static VertexArray create_circle_mesh();

	Vaos& Vaos::getInstance()
	{
		static Vaos instance;
		return instance;
	}

	Vaos::Vaos() :
		point{ 
				{{0,0,0}},
				{3},
				{0}
			},
		quad{
				{ quadVertices, quadUVs },
				quadComponents,
				quadIndices
			},
		plane{ 
				{ planeVertices, planeNormals, planeTangents },
				planeComponents,
				planeIndices },
		cube{
				{ cubeVertices, cubeNormals, cubeTangents },
				cubeComponents,
				cubeIndices
			},
		cubeVNTU{ createCubeVNTUMesh() },
		skyDome{ createSkyDomeMesh(1.0f) },
		circle{ create_circle_mesh() }
	{
	}

	VertexArray Vaos::createCubeVNTUMesh()
	{
		return VertexArray{
			{ cubeVertices, cubeNormals, cubeTangents, cubeUVs },
			{ 3, 3, 3, 2 },
			cubeIndices };
	}

	static VertexArray create_circle_mesh()
	{
		const int circleResolution = 20;
		std::vector<float> positions;
		std::vector<unsigned int> indices;

		positions.push_back(0.0f);
		positions.push_back(0.0f);
		positions.push_back(0.0f);
		for (int i = 0; i <= circleResolution; i++)
		{
			float theta = i * (2 * glm::pi<float>()) / (float)circleResolution;
			positions.push_back(0.5f * cosf(theta)); // x1
			positions.push_back(0.5f * sinf(theta)); // z1
			positions.push_back(0.0f); // y1
		}

		for (int i = 0; i < (positions.size() / 3) - 1; ++i)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i+1);
		}

		return VertexArray{ {positions}, {3}, indices };
	}

	VertexArray Vaos::createSkyDomeMesh(float radius)
	{
		int resolution = 32;
		int latitude = resolution / 2;
		int longitude = resolution;
		int numVertices = longitude * latitude;
		int numIndices = (longitude - 1) * (latitude - 1) * 2 * 3;
		numVertices *= 2;
		numIndices *= 2;

		struct Vertex
		{
			float x, y, z, u, v;
		};

		std::vector<Vertex> vertices;
		vertices.resize(numVertices);

		int domeID = 0;
		for (int i = 0; i < longitude; i++)
		{
			double angleXZ = 100.0f * (i / ((float)longitude - 1.0f)) * glm::pi<float>() / 180.0;

			for (int j = 0; j < latitude; j++)
			{
				double angleY = glm::pi<float>() * j / (latitude - 1);

				vertices[domeID].x = radius * (float)(sin(angleXZ) * cos(angleY));
				vertices[domeID].y = radius * (float)cos(angleXZ);
				vertices[domeID].z = radius * (float)(sin(angleXZ) * sin(angleY));

				vertices[domeID].u = i / (float)longitude;
				vertices[domeID].v = j / (float)latitude;

				domeID++;
			}
		}

		for (int i = 0; i < longitude; i++)
		{
			double angleXZ = 100.0f * (i / ((float)longitude - 1.0f)) * glm::pi<float>() / 180.0;

			for (int j = 0; j < latitude; j++)
			{
				double angleY = (2 * glm::pi<float>()) - (glm::pi<float>() * j / (latitude - 1));

				vertices[domeID].x = radius * (float)(sin(angleXZ) * cos(angleY));
				vertices[domeID].y = radius * (float)cos(angleXZ);
				vertices[domeID].z = radius * (float)(sin(angleXZ) * sin(angleY));

				vertices[domeID].u = i / (float)longitude;
				vertices[domeID].v = j / (float)latitude;

				domeID++;
			}
		}

		std::vector<unsigned int> indices;
		indices.resize(numIndices);

		int index = 0;
		for (size_t i = 0; i < longitude - 1; i++)
		{
			for (size_t j = 0; j < latitude - 1; j++)
			{
				indices[index++] = (unsigned int)(i * latitude + j);
				indices[index++] = (unsigned int)((i + 1) * latitude + j);
				indices[index++] = (unsigned int)((i + 1) * latitude + j + 1);

				indices[index++] = (unsigned int)((i + 1) * latitude + j + 1);
				indices[index++] = (unsigned int)(i * latitude + j + 1);
				indices[index++] = (unsigned int)(i * latitude + j);
			}
		}

		unsigned short off = (unsigned short)(latitude * longitude);
		for (unsigned short i = 0; i < longitude - 1; i++)
		{
			for (unsigned short j = 0; j < latitude - 1; j++)
			{
				indices[index++] = (unsigned short)(off + i * latitude + j);
				indices[index++] = (unsigned short)(off + (i + 1) * latitude + j + 1);
				indices[index++] = (unsigned short)(off + (i + 1) * latitude + j);

				indices[index++] = (unsigned short)(off + i * latitude + j + 1);
				indices[index++] = (unsigned short)(off + (i + 1) * latitude + j + 1);
				indices[index++] = (unsigned short)(off + i * latitude + j);
			}
		}

		std::vector<float> finalPos;
		std::vector<float> finalUv;
		finalPos.reserve(3 * numVertices);
		finalUv.reserve(2 * numVertices);

		for (size_t i = 0; i < numVertices; ++i)
		{
			finalPos.push_back(vertices[i].x);
			finalPos.push_back(vertices[i].y);
			finalPos.push_back(vertices[i].z);

			finalUv.push_back(vertices[i].u);
			finalUv.push_back(vertices[i].v);
		}

		return VertexArray{ {finalPos, finalUv}, {3, 2}, indices };
	}

}