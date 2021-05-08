#include <R3Dpch.h>
#include "../Core/InstancesRenderer.h"

#include "Renderer3D.h"

namespace r3d
{

	using Renderer3DData = std::unordered_map<VertexArray*, std::vector<InstanceSpecs>>;

	static Renderer3DData s_data;
	
	void Renderer3D::init()
	{

	}

	void Renderer3D::push(VertexArray* vao, unsigned int texID, InstanceSpecs&& instanceSpec)
	{
		// vao already there?
		// yes:
		//   
		//   tex already there?
		//   yes:
		//   no:
		// no:
		//
	}

	void Renderer3D::flush()
	{

	}

}