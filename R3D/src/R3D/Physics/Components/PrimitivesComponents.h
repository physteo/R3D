#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct Circle
	{
		Circle(real3 offsetPos = real3{ 0.0f }, rquat offsetRot = fquat{ 1.0, 0.0, 0.0, 0.0 })
			: offsetPos{ offsetPos }, offsetRot{ offsetRot } {}

		real3 offsetPos;
		rquat offsetRot;
	};

	struct Square
	{
		Square(real3 offsetPos = real3{ 0.0f }, rquat offsetRot = fquat{ 1.0, 0.0, 0.0, 0.0 })
			: offsetPos{ offsetPos }, offsetRot{ offsetRot } {}

		real3 offsetPos;
		rquat offsetRot;
	};

	struct Segment
	{
		Segment(real3 offsetPos = real3{ 0.0f }, rquat offsetRot = fquat{ 1.0, 0.0, 0.0, 0.0 })
			: offsetPos{ offsetPos }, offsetRot{ offsetRot } {}

		real3 offsetPos;
		rquat offsetRot;
	};

	struct Plane
	{
		Plane() : normal{ 0.0, 1.0, 0.0 }, offset(0.0f) {}
		Plane(real3 normal, real offset)
			: normal(normal), offset(offset) {}
		
		real3 normal;
		real offset;
	};

	class Sphere
	{
	public:
		Sphere(real3 offsetPos = real3{ 0.0f }, rquat offsetRot = fquat{ 1.0, 0.0, 0.0, 0.0 })
			: offsetPos{ offsetPos }, offsetRot{ offsetRot } {}
		
		real3 offsetPos;
		rquat offsetRot;
	};

	struct Box
	{
		Box(real3 offsetPos = real3{ 0.0f }, rquat offsetRot = fquat{1.0, 0.0, 0.0, 0.0}) 
			: offsetPos{ offsetPos }, offsetRot{ offsetRot } {}

		real3 offsetPos;
		rquat offsetRot;
	};

}