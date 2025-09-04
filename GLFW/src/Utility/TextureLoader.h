#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"
#include <assimp/material.h>
#include "../BufferObjects/Material.h"

class TextureLoader {
private:
	static unsigned int loadTextureFromFile(const char* path, const std::string& directory, bool gamma = false);
	static std::vector<Texture> loadedTextures;
public:
	static unsigned int loadTexture(const std::string& path, bool gamma = false);
	static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		const std::string& typeName,
		const std::string& directory);
	static unsigned int loadCubeMap(const std::vector<std::string>& faces, const std::string& directory, bool gamma = false);
};
