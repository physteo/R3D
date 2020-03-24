#include <R3Dpch.h>

#include "Palette.h"

namespace r3d
{

	Palette& Palette::getInstance()
	{
		static Palette instance;
		return instance;
	}

	float4 Palette::operator()(int id) const
	{
		return colorsVec.at(id);
	}

	Palette::Palette()
	{
		red    = float4{ 190.0f / 255.0f,  34.0f / 255.0f,  34.0f / 255.0f, 1.0f };
		blue   = float4{  19.0f / 255.0f,  50.0f / 255.0f, 255.0f / 255.0f, 1.0f };
		green  = float4{  52.0f / 255.0f,  94.0f / 255.0f,  28.0f / 255.0f, 1.0f };
		orange = float4{ 126.0f / 255.0f,  83.0f / 255.0f,  22.0f / 255.0f, 1.0f };
		grey   = float4{ 100.0f / 255.0f,  100.0f / 255.0f,  100.0f / 255.0f, 1.0f };
		white  = float4{ 1.0f };
		colorsVec = { red, green, blue, orange, grey, white };
	}

}