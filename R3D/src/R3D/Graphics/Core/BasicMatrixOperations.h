#pragma once 

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	float4x4 compute_model_matrix(const float3& position, const fquat& orientation, const float3& scale);

}