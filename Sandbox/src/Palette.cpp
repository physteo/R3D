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

		red = float4{ 205 / 255., 92 / 255.,   92 / 255., 1.0 };
		orange = float4{ 255 / 255., 179 / 255.,  71 / 255., 1.0 };
		green = float4{ 135 / 255., 169 / 255., 107 / 255., 1.0 };
		blue = float4{ 65 / 255., 105 / 255., 225 / 255., 1.0 };
		white = float4{ 1.0f };
		colorsVec = { red, orange, green, blue };
	}

}