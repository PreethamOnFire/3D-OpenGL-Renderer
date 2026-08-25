#include <iostream>
#include "Material.h"

Material::Material(ShaderPipeline* pipeline) : pipeline(pipeline) {
}

void Material::setFloat(const std::string& name, float value) {
	floatProps[name] = value;
}

void Material::setVec3(const std::string& name, const glm::vec3& value) {
	vec3Props[name] = value;
}

void Material::setMat4(const std::string& name, const glm::mat4& value) {
	mat4Props[name] = value;
}

glm::vec3 Material::getVec3OrDefault(const std::string& name, const glm::vec3& def) const {
	auto it = vec3Props.find(name);
	return it != vec3Props.end() ? it->second : def;
}

float Material::getFloatOrDefault(const std::string& name, float def) const {
	auto it = floatProps.find(name);
	return it != floatProps.end() ? it->second : def;
}

glm::vec3 Material::getAmbient() const { return getVec3OrDefault("ambient", glm::vec3(0.0f)); }
glm::vec3 Material::getDiffuse() const { return getVec3OrDefault("diffuse", glm::vec3(0.8f)); }
glm::vec3 Material::getSpecular() const { return getVec3OrDefault("specular", glm::vec3(1.0f)); }
float Material::getShininess() const { return getFloatOrDefault("shininess", 32.0f); }

void Material::setAmbient(const glm::vec3& value) { setVec3("ambient", value); }
void Material::setDiffuse(const glm::vec3& value) { setVec3("diffuse", value); }
void Material::setSpecular(const glm::vec3& value) { setVec3("specular", value); }
void Material::setShininess(float value) { setFloat("shininess", value); }

void Material::setTexture(const std::string& type, const Texture& texture, int slot) {
	if (texture.id == 0) return;

	if (slot < 0) {
		if (type == "diffuse") slot = 0;
		else if (type == "specular") slot = 1;
		else if (type == "normal") slot = 2;
		else slot = 3 + static_cast<int>(textures.size());
	}

	textures.insert_or_assign(type, BoundTexture{ texture, slot });

	if (type == "diffuse")  hasDiffuseMap = true;
	else if (type == "specular") hasSpecularMap = true;
	else if (type == "normal")   hasNormalMap = true;

	textureList.clear();
	for (const auto& [texType, bound] : textures) {
		textureList.push_back(bound.tex);
	}
}

void Material::bind() const {
	pipeline->bind();

	for (const auto& [name, value] : floatProps) pipeline->setFloat("material." + name, value);
	for (const auto& [name, value] : vec3Props)  pipeline->setVec3("material." + name, value);
	for (const auto& [name, value] : mat4Props)  pipeline->setMat4("material." + name, value);

	pipeline->setBool("material.hasDiffuseMap", hasDiffuseMap);
	pipeline->setBool("material.hasSpecularMap", hasSpecularMap);
	pipeline->setBool("material.hasNormalMap", hasNormalMap);

	for (const auto& [type, bound] : textures) {
		if (bound.tex.id == 0) {
			std::cerr << "Warning: Attempting to bind a texture with ID 0." << std::endl;
			continue;
		}
		glActiveTexture(GL_TEXTURE0 + bound.slot);
		glBindTexture(GL_TEXTURE_2D, bound.tex.id);
		pipeline->setInt("material." + type + "0", bound.slot);
	}
}

void Material::unbind() const {
	for (const auto& [type, bound] : textures) {
		glActiveTexture(GL_TEXTURE0 + bound.slot);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	pipeline->restoreState();
}

const std::vector<Texture>& Material::getTextures() const {
	return textureList;
}
