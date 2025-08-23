#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "TextureLoader.h"
#include "../BufferObjects/Material.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

unsigned int TextureLoader::loadTexture(const std::string& path, bool gamma = false) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (!data) {
		std::cerr << "Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
		return 0;
	}

	GLenum format = GL_RGB;
	GLenum internalFormat = gamma ? GL_SRGB : GL_RGB;
	if (nrChannels == 1) {
		format = GL_RED;
		internalFormat = GL_RED;
	} else if (nrChannels == 3) {
		format = GL_RGB;
		internalFormat = gamma ? GL_SRGB : GL_RGB;
	} else if (nrChannels == 4) {
		format = GL_RGBA;
		internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_image_free(data);
	return textureID;
}

unsigned int TextureLoader::loadTextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	return loadTexture(filename, gamma);
}


