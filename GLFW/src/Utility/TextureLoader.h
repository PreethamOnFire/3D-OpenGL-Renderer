#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"

class TextureLoader {
private:
	unsigned int loadTextureFromFile(const char* path, const std::string& directory, bool gamma = false);
public:
	unsigned int loadTexture(const std::string& path, bool gamma = false);
	void loadCubeMap(const std::vector<std::string>& faces, const std::string& directory, bool gamma = false);
};
