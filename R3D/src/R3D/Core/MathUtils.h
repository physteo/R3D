#pragma once

#include <glm.hpp>
#include <iostream>

namespace r3d
{

	typedef double real;
	typedef glm::tvec2<real, glm::precision::highp> real2;
	typedef glm::tvec3<real, glm::precision::highp> real3;
	typedef glm::tvec4<real, glm::precision::highp> real4;
	typedef glm::tmat2x2<real, glm::precision::highp> real2x2;
	typedef glm::tmat3x3<real, glm::precision::highp> real3x3;
	typedef glm::tmat4x4<real, glm::precision::highp> real4x4;

	typedef glm::vec2 float2;
	typedef glm::vec3 float3;
	typedef glm::vec4 float4;
	typedef glm::mat2 float2x2;
	typedef glm::mat3 float3x3;
	typedef glm::mat4 float4x4;

}

std::ostream& operator<<(std::ostream& os, const r3d::float2& v)
{
	return os << "(" << v[0] << ", " << v[1] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::float3& v)
{
	return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::float4& v)
{
	return os << "(" << v.x << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::float2x2& m)
{
	return os << "{" << m[0] << ", " << m[1] << "}";
}

std::ostream& operator<<(std::ostream& os, const r3d::float3x3& m)
{
	return os << "{" << m[0] << ", " << m[1] << ", " << m[2] << "}";
}

std::ostream& operator<<(std::ostream& os, const r3d::float4x4& m)
{
	return os << "{" << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << "}";
}

std::ostream& operator<<(std::ostream& os, const r3d::real2& v)
{
	return os << "("<< v[0] <<", " << v[1] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::real3& v)
{
	return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::real4& v)
{
	return os << "(" << v.x << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
}

std::ostream& operator<<(std::ostream& os, const r3d::real2x2& m)
{
	return os << "{" << m[0] << ", " << m[1] << "}";
}

std::ostream& operator<<(std::ostream& os, const r3d::real3x3& m)
{
	return os << "{" << m[0] << ", " << m[1] << ", " << m[2] << "}";
}

std::ostream& operator<<(std::ostream& os, const r3d::real4x4& m)
{
	return os << "{" << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << "}";
}