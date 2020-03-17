#include <R3Dpch.h>
#include <R3D/Core/MathUtils.h>

/* opengl */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ImGuiCustom.h"

namespace ImGui
{
	bool SliderUniformFloatOpenGL(unsigned int shaderID, const char* uniform, const char* label, float* v, float v_min, float v_max, const char* format, float power)
	{
		float newValue = *v;
		bool result = ImGui::SliderFloat(label, &newValue, v_min, v_max, format, power);
		if (glm::abs(newValue - *v) > R3D_EPSILON)
		{
			glUseProgram(shaderID);

			int location;
			location = glGetUniformLocation(shaderID, uniform); // TODO: if location not found, log a warning
			glUniform1f(location, newValue);

			glUseProgram(0);
			*v = newValue;
		}

		return result;
	}

}