#include <R3Dpch.h>
#include "MathUtils.h"

namespace r3d
{
	real3x3 change_basis_of_matrix(const rquat& q, const real3x3& mat)
	{
		rquat invq = q;
		invq.x = -q.x;
		invq.y = -q.y;
		invq.z = -q.z;

		return glm::toMat3(q) * mat * glm::toMat3(invq);

	}
}