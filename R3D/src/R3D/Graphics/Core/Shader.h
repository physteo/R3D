#pragma once

/* opengl */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string GeometrySource;
		std::string ComputeShader;
	};
	static ShaderProgramSource ParseShader(const std::string& filepath);

	class Shader
	{
	public:
		Shader() : m_id(0), m_path("") {};
		Shader(const std::string& path) : m_id(0), m_path("")
		{
			generate(path);
		};

		//Cannot use the copy constructor/assignment.
		Shader(const Shader &) = delete;
		Shader& operator=(const Shader &) = delete;

		//Can use move constructor/assignment.
		Shader(Shader&& other);
		Shader& operator=(Shader&& other);

		void bind()   const { bind(m_id); }
		static void bind(unsigned int id) { glUseProgram(id); }
		static void unbind() { glUseProgram(0); }

		void setUniformValue(const std::string& name, int          value) const;
		void setUniformValue(const std::string& name, double       value) const;
		void setUniformValue(const std::string& name, unsigned int value) const;
		void setUniformValue(const std::string& name, float value) const;
		void setUniformValue(const std::string& name, float v1, float v2) const;
		void setUniformValue(const std::string& name, float v1, float v2, float v3) const;
		void setUniformValue(const std::string& name, float v1, float v2, float v3, float v4) const;
		void setUniformValue(const std::string& name, float3) const;
		void setUniformMatrix(const std::string& name, const float4x4& matrix, bool transpose) const;

		static void setUniformValue(unsigned int id, const std::string& name, int          value);
		static void setUniformValue(unsigned int id, const std::string& name, double       value);
		static void setUniformValue(unsigned int id, const std::string& name, unsigned int value);
		static void setUniformValue(unsigned int id, const std::string& name, float value);
		static void setUniformValue(unsigned int id, const std::string& name, float v1, float v2);
		static void setUniformValue(unsigned int id, const std::string& name, float v1, float v2, float v3);
		static void setUniformValue(unsigned int id, const std::string& name, float v1, float v2, float v3, float v4);
		static void setUniformValue(unsigned int id, const std::string& name, float3);
		static void setUniformMatrix(unsigned int id, const std::string& name, const float4x4& matrix, bool transpose);

		void setTexture(GLenum target, const std::string& uniformName, unsigned int textureID);
		inline unsigned int getID() const { return m_id; }

	private:
		void generate(const std::string& path);
		unsigned int compileShader(unsigned int type, const std::string& shader);
		void release();
		void swapData(Shader& other);

		unsigned int m_id;
		std::string  m_path;
		std::vector<std::string> m_textureUnits;

	};

}