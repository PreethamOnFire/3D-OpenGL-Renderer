#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Core/ShaderPipeline.h"

struct Texture {
	GLuint id;
	std::string type; // e.g., "diffuse", "specular"
	std::string path; // file path for loading the texture

	Texture(GLuint textureID, const std::string& textureType, const std::string& texturePath)
		: id(textureID), type(textureType), path(texturePath) {
	}
};

class Material {
public:
	explicit Material(ShaderPipeline* pipeline);

	ShaderPipeline* getPipeline() const { return pipeline; }

	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setMat4(const std::string& name, const glm::mat4& value);
	void setTexture(const std::string& type, const Texture& texture, int slot = -1);

	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;
	float getShininess() const;
	void setAmbient(const glm::vec3& value);
	void setDiffuse(const glm::vec3& value);
	void setSpecular(const glm::vec3& value);
	void setShininess(float value);

	bool isTransparent() const { return transparent; }
	void setTransparent(bool value) { transparent = value; }

	void bind() const;
	void unbind() const;

	const std::vector<Texture>& getTextures() const;

private:
	struct BoundTexture {
		Texture tex;
		int slot;
	};

	glm::vec3 getVec3OrDefault(const std::string& name, const glm::vec3& def) const;
	float getFloatOrDefault(const std::string& name, float def) const;

	ShaderPipeline* pipeline;

	std::unordered_map<std::string, float> floatProps;
	std::unordered_map<std::string, glm::vec3> vec3Props;
	std::unordered_map<std::string, glm::mat4> mat4Props;
	std::unordered_map<std::string, BoundTexture> textures;
	std::vector<Texture> textureList; // kept in sync with `textures`, for getTextures()

	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasNormalMap = false;
	bool transparent = false;
};
