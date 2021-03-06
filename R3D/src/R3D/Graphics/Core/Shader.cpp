#include <R3Dpch.h>
#include <R3D/Core/Log.h>

#include "Shader.h"

namespace r3d
{

	ShaderProgramSource ParseShader(const std::string& filepath)
	{
		bool geometryShaderExists = false;
		bool computeShaderExists = false;

		std::ifstream stream(filepath);
		std::string line = "";
		std::stringstream ss[4];
		ss[0].str(std::string());
		ss[1].str(std::string());
		ss[2].str(std::string());
		ss[3].str(std::string());

		ShaderType type = ShaderType::NONE;
		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}
				else if (line.find("geometry") != std::string::npos)
				{
					geometryShaderExists = true;
					type = ShaderType::GEOMETRY;
				}
				else if (line.find("compute"))
				{
					computeShaderExists = true;
					type = ShaderType::COMPUTE;
				}
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}

		ShaderProgramSource source = { ss[0].str(), ss[1].str(), geometryShaderExists ? ss[2].str() : "",  computeShaderExists ? ss[3].str() : "" };
		return source;
	}

	Shader::Shader() : m_id(0), m_path("") {};

	Shader::Shader(const std::string& path) : m_id(0), m_path(path)
	{
		ShaderProgramSource source = ParseShader(path);
		unsigned int id = generateProgram(source);
		if (id != 0)
		{
			m_id = id;
			m_source = source;
		}
		else
		{
			m_id = 0;
		}
	};

	Shader::Shader(unsigned int id, ShaderProgramSource source, const std::string& path)
	{
		m_path = path;
		m_id = id;
		m_source = std::move(source);
	}

	Shader::Shader(Shader&& other)
	{
		swapData(other);
	}
	Shader& Shader::operator=(Shader&& other)
	{
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	unsigned int generateProgram(ShaderProgramSource source)
	{
		std::string vertexShader = source.VertexSource;
		std::string fragmentShader = source.FragmentSource;
		std::string geometryShader = source.GeometrySource;
		std::string computeShader = source.ComputeShader;

		/* create and compile vertex, fragment and geometry (if present) shaders */
		unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

		if (vs == 0 || fs == 0)
		{
			return 0;
		}

		unsigned int gs = -1;
		if (geometryShader != "") {
			gs = compileShader(GL_GEOMETRY_SHADER, geometryShader);
		}

		unsigned int cs = -1;
		if (computeShader != "")
		{
			cs = compileShader(GL_COMPUTE_SHADER, computeShader);
		}

		/* create a program and attach the shaders */
		unsigned int id = glCreateProgram();

		// is there a compute shader? discard the rest
		if (computeShader != "")
		{
			glAttachShader(id, cs);
		}
		else
		{
			glAttachShader(id, vs);
			glAttachShader(id, fs);
			if (geometryShader != "") {
				glAttachShader(id, gs);
			}
		}

		/* link the program */
		glLinkProgram(id);

		/* validate the program */
		//glValidateProgram(id); //TODO: needed?

		/* now the shaders' memory can be freed */
		if (computeShader != "")
		{
			glDeleteShader(cs);
		}
		else
		{
			glDeleteShader(vs);
			glDeleteShader(fs);
			if (geometryShader != "") {
				glDeleteShader(gs);
			}
		}

		/* check the status of linking and print log if linking failed */
		GLint linked;
		glGetProgramiv(id, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE)
		{
			// retrieve the length of the log
			GLint logLength;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

			// retrieve the log
			char* log = (char*)alloca(logLength);
			glGetProgramInfoLog(id, logLength, nullptr, log);

			// print the log
			std::cout << "Failure at linking program. Log: \n" << log << std::endl;
			return 0;
		}

		return id;
	}

	void Shader::setTexture(GLenum target, const std::string& uniformName, unsigned int textureID)
	{
		std::vector<std::string>::iterator it = std::find(m_textureUnits.begin(), m_textureUnits.end(), uniformName);
		int textureUnit;
		if (it == m_textureUnits.end())
		{
			// new texture
			textureUnit = (int)m_textureUnits.size();
#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
			int maxTextureUnits = 0;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
			R3D_CORE_ASSERT(textureUnit < maxTextureUnits, "[Shader]: maximum number of texture units {0} exceeded.",
					maxTextureUnits);
#endif
			m_textureUnits.push_back(uniformName);
			setUniformValue(uniformName, textureUnit);
		}
		else
		{
			// texture unit already exists, dont set the uniform
			textureUnit = (int)(it - m_textureUnits.begin());
		}

		glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
		glBindTexture(target, textureID);
	}

	unsigned int compileShader(unsigned int type, const std::string& shader)
	{
		unsigned int s;
		const char* src = shader.c_str();
		s = glCreateShader(type);
		GLint length = static_cast<GLint>(strlen(src));
		glShaderSource(s, 1, &src, &length);
		glCompileShader(s);

		GLint result;
		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{

			glGetShaderiv(s, GL_INFO_LOG_LENGTH, &result);

			char* log = (char*)alloca(result);
			glGetShaderInfoLog(s, result, nullptr, log);

			const char* typeToLog = "unspecified";
			switch (type) {
			case GL_VERTEX_SHADER: typeToLog = "vertex"; break;
			case GL_FRAGMENT_SHADER: typeToLog = "fragment"; break;
			case GL_GEOMETRY_SHADER: typeToLog = "geometry"; break;
			case GL_COMPUTE_SHADER: typeToLog = "compute"; break;
			}

			std::cerr << "Failure at compiling " << typeToLog << " shader. Log: \n" << log << std::endl;
			return 0;
		}
		return s;
	}

	void Shader::release()
	{
		glDeleteProgram(m_id);
		m_id = 0;
		m_path = "";
		m_textureUnits.clear();
		m_uniformLocations.clear();
	}

	void Shader::swapData(Shader& other)
	{
		m_id = other.m_id;
		m_path = other.m_path;
		m_source = other.m_source;
		m_textureUnits = std::move(other.m_textureUnits);
		m_uniformLocations = std::move(other.m_uniformLocations);

		other.m_id = 0;
		other.m_path = "";
		other.m_textureUnits.clear();
		other.m_source.VertexSource = "";
		other.m_source.FragmentSource = "";
		other.m_source.GeometrySource = "";
		other.m_source.ComputeShader = "";
	}

	void Shader::setUniformValue(const std::string& name, int value) const
	{
		glUniform1i(getUniformLocation(name), value);
	}

	void Shader::setUniformValue(const std::string& name, unsigned int value) const
	{
		glUniform1ui(getUniformLocation(name), value);
	}

	void Shader::setUniformValue(const std::string& name, float        value) const
	{
		glUniform1f(getUniformLocation(name), value);
	}

	void Shader::setUniformValue(const std::string & name, float v1, float v2) const
	{
		glUniform2f(getUniformLocation(name), v1, v2);
	}

	void Shader::setUniformValue(const std::string & name, float v1, float v2, float v3) const
	{
		glUniform3f(getUniformLocation(name), v1, v2, v3);
	}

	void Shader::setUniformValue(const std::string & name, float v1, float v2, float v3, float v4) const
	{
		glUniform4f(getUniformLocation(name), v1, v2, v3, v4);
	}

	void Shader::setUniformValue(const std::string& name, const float3& value) const
	{
		setUniformValue(name, value[0], value[1], value[2]);
	}

	void Shader::setUniformValue(const std::string& name, const float4& value) const
	{
		setUniformValue(name, value[0], value[1], value[2], value[3]);
	}

	void Shader::setUniformMatrix(const std::string& name, const float4x4& matrix, bool transpose) const
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, transpose, glm::value_ptr(matrix));
	}

	int Shader::getUniformLocation(const std::string& name) const
	{
		auto found = m_uniformLocations.find(name);
		if (found != m_uniformLocations.end())
		{
			return found->second;
		}
		else
		{
			int location;
			location = glGetUniformLocation(m_id, name.c_str());
			m_uniformLocations.insert({ name, location });

			return location;
		}
	}

}