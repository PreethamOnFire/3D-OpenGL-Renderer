#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"
#include "Material.h"

void Material::addTexture(const Texture& texture) {
	textures.push_back(texture);
}

void Material::bindTextures(Shader& shader) const {
	for (size_t i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
}

void Material::unbindTextures() const {
	for (size_t i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0); 
}

void Material::setUniforms(Shader& shader) const {
	shader.setVec3("material.ambient", ambient);
	shader.setVec3("material.diffuse", diffuse);
	shader.setVec3("material.specular", specular);
	shader.setFloat("material.shininess", shininess);

	unsigned int diffuseCount = 0;
	unsigned int specularCount = 0;
	unsigned int normalCount = 0;

	for (size_t i = 0; i < textures.size(); ++i) {
		std::string number;
		std::string type = textures[i].type;

		if (type == "diffuse") {
			number = std::to_string(diffuseCount++);
		}
		else if (type == "specular") {
			number = std::to_string(specularCount++);
		}
		else if (type == "normal") {
			number = std::to_string(normalCount++);
		}
		std::string uniformName = type + number;
		shader.setInt(uniformName, i);
	}
}

const std::vector<Texture>& Material::getTextures() const {
	return textures;
}