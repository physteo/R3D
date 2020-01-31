#pragma once
// give it a shader
// give it a vao
// give it a vector of instances
// it renders using instancing

#include "Shader.h"
#include "VertexArray.h"

#include <vector>

namespace r3d
{
	struct ShapeData
	{
		VertexArray  vao;
		Buffer databuffer;
		unsigned int numberOfIndices;
	};

	class InstancesRenderer
	{
	public:
		template<class InstanceData>
		static void render(const std::vector<InstanceData>& instancesData, const ShapeData& shapeData, GLenum mode)
		{
			if (instancesData.size() != 0)
			{
				auto& databuffer = shapeData.databuffer;
				auto& vao = shapeData.vao;
				unsigned int numIndices = shapeData.numberOfIndices;
				size_t instancesNumber = instancesData.size();

				// pass data
				databuffer.bind();
				glBufferData(GL_ARRAY_BUFFER, instancesNumber * sizeof(InstanceData), &(instancesData[0]), GL_DYNAMIC_DRAW);

				vao.bind();
				glDrawElementsInstanced(mode, numIndices, GL_UNSIGNED_INT, 0, instancesNumber);
				vao.unbind();
			}
		}

	};

}