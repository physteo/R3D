#include <R3Dpch.h>
#include <R3D/Core/Log.h>

#include "Material.h"
#include "load_image.h"

namespace r3d
{

	static unsigned int generate_texture(int width, int height, int nrChannels, TextureFormat internalFormat, GLenum dataFormat, const unsigned char* data)
	{
		TextureFormat format;
		if (nrChannels == 4)
		{
			format = TextureFormat::RGBA;
		}
		else if (nrChannels == 3)
		{
			format = TextureFormat::RGB;
		}
		else
		{
			R3D_CORE_ASSERT("Texture format not supported (nrChannels: {0}).", nrChannels);
		}

		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataFormat, (void*)data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return id;
	}

	unsigned int Material::loadTexture(const std::string& filepath)
	{
		unsigned int id;
		int width, height, nrChannels;
		unsigned char* data = load_image(filepath.c_str(), &width, &height, &nrChannels, false);
		if (data)
		{
			id = generate_texture(width, height, nrChannels, m_internalFormat, GL_UNSIGNED_BYTE, data);
			free_image(data);
			return id;
		}
		else
		{
			return 0;
		}
	}

	Material::Material()
	{
		m_diffuseID  = 0;
		m_specularID = 0;
		m_normalID   = 0;
		m_shininess = 0; 
	}

	Material::Material(const std::string& diffuseTexture, const std::string& specularTexture,
		const std::string& normalTexture, float shininess)
	{
		m_diffuseID = loadTexture(diffuseTexture);
		m_specularID = loadTexture(specularTexture);
		m_normalID = loadTexture(normalTexture);
		m_shininess = shininess;

		R3D_CORE_ASSERT(m_diffuseID != 0, "Failed to load diffuse texture at '{0}'", diffuseTexture);
		R3D_CORE_ASSERT(m_specularID != 0, "Failed to load specular texture at '{0}'", specularTexture);
		R3D_CORE_ASSERT(m_normalID != 0, "Failed to load normal texture at '{0}'", normalTexture);
	}

	Material::Material(Material&& other)
	{
		swapData(other);
	}

	Material& Material::operator=(Material&& other)
	{
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	Material::~Material()
	{
		release();
	}

	void Material::release()
	{
		glDeleteTextures(1, &m_diffuseID);
		glDeleteTextures(1, &m_specularID);
		glDeleteTextures(1, &m_normalID);
	}

	void Material::swapData(Material& other)
	{
		m_diffuseID = other.m_diffuseID;
		m_specularID = other.m_specularID;
		m_normalID = other.m_normalID;
		m_shininess = other.m_shininess;

		other.m_diffuseID  = 0;
		other.m_specularID = 0;
		other.m_normalID   = 0;
		other.m_shininess  = 0;
	}
		
	void Material::passUniforms(Shader& shader) const
	{
		shader.setTexture(GL_TEXTURE_2D, "material.diffuse",  m_diffuseID);
		shader.setTexture(GL_TEXTURE_2D, "material.specular", m_specularID);
		shader.setTexture(GL_TEXTURE_2D, "material.normal",   m_normalID);
		shader.setUniformValue("material.shininess", m_shininess);
	}

}