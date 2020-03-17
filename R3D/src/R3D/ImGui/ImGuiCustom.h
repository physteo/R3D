#pragma once

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ImGui
{

	IMGUI_API bool SliderUniformFloatOpenGL(unsigned int shaderID, const char* uniform, const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	//IMGUI_API bool SliderShaderUniformFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	//IMGUI_API bool SliderShaderUniformFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	//IMGUI_API bool SliderShaderUniformFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);

}