#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "TextureLoader.h"
#include "../BufferObjects/Material.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#include <assimp/material.h>

unsigned int TextureLoader::loadTexture(const std::string& path, bool gamma) {
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
	unsigned int textureID = loadTexture(filename, gamma);
	if (textureID == 0) {
		std::vector<std::string> searchPaths = {
			directory + "/textures/" + filename,
			directory + "/materials/" + filename,
			directory + "/maps/" + filename,
			directory + "/textures/maps/" + filename,
			directory + "/../textures/" + filename,
			directory + "/../materials/" + filename
		};

		for (const auto& searchPath : searchPaths) {
			textureID = loadTexture(searchPath, gamma);
			if (textureID != 0) {
				std::cout << "Found texture at: " << searchPath << std::endl;
				break;
			}
		}
	}

	if (textureID == 0) {
		std::cerr << "TextureLoader: Failed to load texture at path: " << filename << std::endl;
	}
	return textureID;
}

std::vector<Texture> TextureLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
	const std::string& typeName,
	const std::string& directory)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++) {
			if (std::strcmp(loadedTextures[j].path.c_str(), str.C_Str()) == 0) {
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture = Texture(loadTextureFromFile(str.C_Str(), directory), typeName, str.C_Str());
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureLoader::loadCubeMap(const std::vector<std::string>& faces, const std::string& directory, bool gamma) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		std::string path = directory + '/' + faces[i];
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (!data) {
			std::cerr << "Failed to load texture at path: " << path << std::endl;
			stbi_image_free(data);
			return 0;
		}

		GLenum format = GL_RGB;
		if (nrChannels == 1) format = GL_RED;
		else if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (textureID == 0) {
		std::cerr << "TextureLoader: Failed to load cubemap at directory: " << directory << std::endl;
	}

	return textureID;
}


