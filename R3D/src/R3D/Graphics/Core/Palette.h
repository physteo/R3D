#pragma once
#include <R3D/Core/MathUtils.h>

namespace r3d
{

	class Palette
	{
	public:
		float4 red;
		float4 orange;
		float4 green;
		float4 blue;
		float4 grey;
		float4 white;
	
	public:
		static Palette& getInstance();
		float4 operator()(int id) const;
		inline size_t size() const { return colorsVec.size(); }

		inline void setRed(float4 color) { red = color; }
		inline void setBlue(float4 color) { blue = color; }
		inline void setOrange(float4 color) { orange = color; }
		inline void setGreen(float4 color) { green = color; }
		inline void setGrey(float4 color) { grey = color; }

	private:
		Palette();

	private:
		std::vector<float4> colorsVec;

	};

}