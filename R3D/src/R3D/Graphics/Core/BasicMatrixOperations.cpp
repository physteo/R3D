#include <R3Dpch.h>
#include "BasicMatrixOperations.h"

namespace r3d
{

	float4x4 compute_model_matrix(const float3& position, const fquat& orientation, const float3& scale)
	{
		float4x4 modelMatrix{ 1.0 };
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = modelMatrix * glm::toMat4(orientation);
		modelMatrix = glm::scale(modelMatrix, scale);

		return modelMatrix;
	}

	

}