#pragma once
#include <R3D/Core/MathUtils.h>
#include <vector>

namespace r3d
{

	class Palette
	{
	public:
		float4 red;
		float4 orange;
		float4 green;
		float4 blue;
		float4 white;
	
	public:
		static Palette& getInstance();
		float4 operator()(int id) const;
		inline size_t size() const { return colorsVec.size(); }

	private:
		Palette();

	private:
		std::vector<float4> colorsVec;

	};

}