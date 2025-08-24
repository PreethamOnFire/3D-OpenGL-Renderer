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
	int diffuseUnit = 0, specularUnit = 1, normalUnit = 2;
	for (const auto& texture : textures) {
		if (texture.id == 0) continue;
		if (texture.type == "diffuse") {
			std::cout << "Binding diffuse texture ID " << texture.id << " to unit " << diffuseUnit << std::endl;
			glActiveTexture(GL_TEXTURE0 + diffuseUnit);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
		else if (texture.type == "specular") {
			std::cout << "Binding specular texture ID " << texture.id << " to unit " << specularUnit << std::endl;
			glActiveTexture(GL_TEXTURE0 + specularUnit);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
		else if (texture.type == "normal") {
			std::cout << "Binding normal texture ID " << texture.id << " to unit " << normalUnit << std::endl;
			glActiveTexture(GL_TEXTURE0 + normalUnit);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
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

	bool hasDiffuse = false, hasSpecular = false, hasNormal = false;

	for (const auto& texture : textures) {
		if (texture.type == "diffuse") hasDiffuse = true;
		else if (texture.type == "specular") hasSpecular = true;
		else if (texture.type == "normal") hasNormal = true;
	}

	shader.setBool("material.hasDiffuseMap", hasDiffuse);
	shader.setBool("material.hasSpecularMap", hasSpecular);
	shader.setBool("material.hasNormalMap", hasNormal);

	// Set texture samplers
	if (hasDiffuse) shader.setInt("material.diffuse0", 0);
	if (hasSpecular) shader.setInt("material.specular0", 1);
	if (hasNormal) shader.setInt("material.normal0", 2);
}

const std::vector<Texture>& Material::getTextures() const {
	return textures;
}