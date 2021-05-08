#pragma once

#include "FrameBuffer.h"
#include "Shader.h"

namespace r3d
{



	class Material
	{
	public:
		Material();
		Material(const std::string& diffuseTexture, const std::string& normalTexture,
			const std::string& specularTexture, float shininess);
		~Material();
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;
		Material(Material&& other);
		Material& operator=(Material&& other);

		void passUniforms(Shader& shader) const;
		inline unsigned int getDiffuse()  const { return m_diffuseID; }
		inline unsigned int getNormal()   const { return m_normalID; }
		inline unsigned int getSpecular() const { return m_specularID; }
		inline float getShininess() const { return m_shininess; }

	private:
		// returns 0 if loading fails
		unsigned int loadTexture(const std::string& filepath);
		void release();
		void swapData(Material& other);

		const TextureFormat m_internalFormat{ TextureFormat::RGBA };

		unsigned int m_diffuseID;
		unsigned int m_normalID;
		unsigned int m_specularID;
		float m_shininess;
	};

}