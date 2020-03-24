#include <R3Dpch.h>
#include "MathUtils.h"

namespace r3d
{

	// From : http://box2d.org/2014/02/computing-a-basis/
	void compute_basis(const real3& normal, real3& tangent, real3& bitangent)
	{
		if (glm::abs(normal.x) >= 0.57735027) // TODO: put into a const sqrt(3)
		{
			tangent = { normal.y, -normal.x, 0.0 };
		}
		else
		{
			tangent = { 0.0, normal.z, -normal.y };
		}

		tangent = glm::normalize(tangent);
		bitangent = glm::cross(normal, tangent);
	}

	real3x3 change_basis_of_matrix(const rquat& q, const real3x3& mat)
	{
		rquat invq = q;
		invq.x = -q.x;
		invq.y = -q.y;
		invq.z = -q.z;

		return glm::toMat3(q) * mat * glm::toMat3(invq);
	}

	float4x4 compute_model_matrix(const float3& position, const fquat& orientation, const float3& scale)
	{
		float4x4 modelMatrix{ 1.0 };
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = modelMatrix * glm::toMat4(orientation);
		modelMatrix = glm::scale(modelMatrix, scale);

		return modelMatrix;
	}

}