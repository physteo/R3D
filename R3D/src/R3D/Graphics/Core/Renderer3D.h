#pragma once

namespace r3d
{

	struct InstanceSpecs
	{
		float4x4 modelMatrix;
		float4x4 normalMatrix;
		float4 color;
		int texId;
	};

	class Renderer3D
	{
	public:
		static void init();
		static void push(VertexArray* vao, unsigned int texID, InstanceSpecs&& instanceSpec);
		static void flush();
		
	};

}