#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Core/Shader.h"
#include "Material.h"

void Material::addTexture(const Texture& texture) {
	if (texture.id == 0) return;
	textures.push_back(texture);
	if (texture.type == "diffuse")  hasDiffuseMap = true;
	else if (texture.type == "specular") hasSpecularMap = true;
	else if (texture.type == "normal")   hasNormalMap = true;
}

void Material::bindTextures(Shader& shader) const {
	int diffuseUnit = 0, specularUnit = 1, normalUnit = 2;
	for (const auto& texture : textures) {
		if (texture.id == 0) {
			std::cerr << "Warning: Attempting to bind a texture with ID 0." << std::endl;
			continue;
		}
		if (texture.type == "diffuse") {
			glActiveTexture(GL_TEXTURE0 + diffuseUnit);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
		else if (texture.type == "specular") {
			glActiveTexture(GL_TEXTURE0 + specularUnit);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
		else if (texture.type == "normal") {
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

	shader.setBool("material.hasDiffuseMap", hasDiffuseMap);
	shader.setBool("material.hasSpecularMap", hasSpecularMap);
	shader.setBool("material.hasNormalMap", hasNormalMap);

	if (hasDiffuseMap)  shader.setInt("material.diffuse0", 0);
	if (hasSpecularMap) shader.setInt("material.specular0", 1);
	if (hasNormalMap)   shader.setInt("material.normal0", 2);
}

const std::vector<Texture>& Material::getTextures() const {
	return textures;
}