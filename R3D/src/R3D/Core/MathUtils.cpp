#include <R3Dpch.h>
#include "MathUtils.h"

namespace r3d
{
	real3x3 change_basis_of_matrix(const rquat& q, const real3x3& mat)
	{
		
		//rquat v0{ 1,0,0,0 };
		//rquat v1{ 0.0, mat[0][0], mat[0][1], mat[0][2] };
		//rquat v2{ 0.0, mat[1][0], mat[1][1], mat[1][2] };
		//rquat v3{ 0.0, mat[2][0], mat[2][1], mat[2][2] };
		//
		//// left multiplication q * Mat
		//rquat v0temp = q * v0;
		//rquat v1temp = q * v1;
		//rquat v2temp = q * v2;
		//rquat v3temp = q * v3;
		//
		//// right multiplication (q * Mat) * q^-1
		//rquat invq = q;
		//invq.x = -q.x;
		//invq.y = -q.y;
		//invq.z = -q.z;
		//
		//rquat v0result = v0temp * invq;
		//rquat v1result = v1temp * invq;
		//rquat v2result = v2temp * invq;
		//rquat v3result = v3temp * invq;
		//
		//real3x3 resultMat;
		//resultMat[0] = { v1result.x, v1result.y, v1result.z };
		//resultMat[1] = { v2result.x, v2result.y, v2result.z };
		//resultMat[2] = { v3result.x, v3result.y, v3result.z };
		//return resultMat;

		rquat invq = q;
		invq.x = -q.x;
		invq.y = -q.y;
		invq.z = -q.z;

		return glm::toMat3(q) * mat * glm::toMat3(invq);

	}
}