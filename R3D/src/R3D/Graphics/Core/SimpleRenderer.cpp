#include <R3Dpch.h>
#include "SimpleRenderer.h"
#include "Shader.h"

namespace r3d
{
	static Shader s_debugShader{ "C:/Users/matte/source/repos/OpenGL/Rendara3D/res/shaders/lines.shader" };

	static void debug_draw_segment(float3 origin, float3 vector, float3 color)
	{
		float len = glm::length(vector);
		 
		std::vector<std::vector<float> > attributes = 
		{
			{0.0, 0.0, 0.0, 1.0, 0.0, 0.0}
		};
		std::vector<unsigned int> components = {3};
		std::vector<unsigned int> indices = {0, 1};


		VertexArray vao{attributes, components, indices};

		s_debugShader.bind();

		

		s_debugShader.unbind();
	}


}