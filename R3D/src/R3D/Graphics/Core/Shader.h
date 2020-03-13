#pragma once

/* opengl */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, COMPUTE = 3
	};

	struct ShaderProgramSource
	{
		ShaderProgramSource(
			const std::string& vertex = "",
			const std::string& fragment = "",
			const std::string& geometry = "",
			const std::string& compute = "") :
			VertexSource{ vertex },
			FragmentSource{ fragment },
			GeometrySource{ geometry },
			ComputeShader{ compute } {}
		std::string VertexSource;
		std::string FragmentSource;
		std::string GeometrySource;
		std::string ComputeShader;
	};

	ShaderProgramSource ParseShader(const std::string& filepath);

	unsigned int compileShader(unsigned int type, const std::string& shader);

	unsigned int generateProgram(ShaderProgramSource source);


	class Shader
	{
	public:
		Shader();
		Shader(const std::string& path);
		Shader(unsigned int id, ShaderProgramSource source, const std::string& path);

		//Cannot use the copy constructor/assignment.
		Shader(const Shader &) = delete;
		Shader& operator=(const Shader &) = delete;

		//Can use move constructor/assignment.
		Shader(Shader&& other);
		Shader& operator=(Shader&& other);

		void bind()   const { bind(m_id); }
		static void bind(unsigned int id) { glUseProgram(id); }
		static void unbind() { glUseProgram(0); }

		inline const std::string& getFilename() const { return m_path; }
		inline const ShaderProgramSource& getSource() const { return m_source; }

		void setUniformValue(const std::string& name, int          value) const;
		void setUniformValue(const std::string& name, double       value) const;
		void setUniformValue(const std::string& name, unsigned int value) const;
		void setUniformValue(const std::string& name, float value) const;
		void setUniformValue(const std::string& name, float v1, float v2) const;
		void setUniformValue(const std::string& name, float v1, float v2, float v3) const;
		void setUniformValue(const std::string& name, float v1, float v2, float v3, float v4) const;
		void setUniformValue(const std::string& name, float3 value) const;
		void setUniformValue(const std::string& name, float4 value) const;
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
		void release();
		void swapData(Shader& other);

		unsigned int m_id;
		std::string  m_path;
		std::vector<std::string> m_textureUnits;
		ShaderProgramSource m_source;

	};

}