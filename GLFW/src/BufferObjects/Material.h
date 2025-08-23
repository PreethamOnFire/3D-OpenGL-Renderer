#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"

struct Texture {
	GLuint id;
	std::string type; // e.g., "diffuse", "specular"
	std::string path; // file path for loading the texture

	Texture(GLuint textureID, const std::string& textureType, const std::string& texturePath)
		: id(textureID), type(textureType), path(texturePath) {
	}
};

class Material {
private:
	std::vector<Texture> textures;

public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material() : ambient(0.2f), diffuse(0.8f), specular(1.0f), shininess(32.0f) {}

	void addTexture(const Texture& texture);

	void bindTextures(Shader& shader) const;

	void unbindTextures() const;

	void setUniforms(Shader& shader) const;

	const std::vector<Texture>& getTextures() const;
};


