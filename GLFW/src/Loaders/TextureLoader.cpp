#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "TextureLoader.h"

std::vector<Texture> TextureLoader::loadedTextures;
#include "../Rendering/Material.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"
#include <assimp/material.h>
#include <vector>
#include <filesystem>

void TextureLoader::clearCache() {
	for (const auto& texture : loadedTextures) {
		glDeleteTextures(1, &texture.id);
	}
	loadedTextures.clear();
}

bool TextureLoader::channelsToFormat(int channels, bool gamma, GLenum& format, GLenum& internalFormat) {
	if (channels == 1) {
		format = internalFormat = GL_RED;
	}
	else if (channels == 3) {
		format = GL_RGB;
		internalFormat = gamma ? GL_SRGB : GL_RGB;
	}
	else if (channels == 4) {
		format = GL_RGBA;
		internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
	}
	else {
		return false;
	}
	return true;
}

void TextureLoader::uploadTexture2D(unsigned int textureID, int width, int height, GLenum format, GLenum internalFormat, const unsigned char* data) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Essential fix!
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

unsigned int TextureLoader::loadTexture(const std::string& path, bool gamma) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (!data) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		return 0;
	}

	GLenum format, internalFormat;
	if (!channelsToFormat(nrChannels, gamma, format, internalFormat)) {
		std::cerr << "TextureLoader: unsupported channel count " << nrChannels << " in " << path << std::endl;
		stbi_image_free(data);
		return 0;
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);
	uploadTexture2D(textureID, width, height, format, internalFormat, data);

	stbi_image_free(data);
	return textureID;
}

unsigned int TextureLoader::loadEmbeddedTexture(const aiTexture* texture, bool gamma) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (texture->mHeight == 0) {
		// Compressed image (PNG/JPEG/etc.) stored as a single blob -- the common case for
		// glTF/GLB, where materials reference scene->mTextures instead of a file on disk.
		// mWidth here is the blob's byte length, not a pixel width.
		int width, height, nrChannels;
		const unsigned char* compressed = reinterpret_cast<const unsigned char*>(texture->pcData);
		unsigned char* data = stbi_load_from_memory(compressed, static_cast<int>(texture->mWidth), &width, &height, &nrChannels, 0);
		if (!data) {
			std::cerr << "Failed to decode embedded texture (format hint: " << texture->achFormatHint << ")" << std::endl;
			glDeleteTextures(1, &textureID);
			return 0;
		}

		GLenum format, internalFormat;
		if (!channelsToFormat(nrChannels, gamma, format, internalFormat)) {
			std::cerr << "TextureLoader: unsupported channel count " << nrChannels << " in embedded texture" << std::endl;
			stbi_image_free(data);
			glDeleteTextures(1, &textureID);
			return 0;
		}

		uploadTexture2D(textureID, width, height, format, internalFormat, data);
		stbi_image_free(data);
	}
	else {
		// Uncommon: raw uncompressed texel array, width x height, BGRA order (aiTexel).
		GLenum internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
		uploadTexture2D(textureID, static_cast<int>(texture->mWidth), static_cast<int>(texture->mHeight),
			GL_BGRA, internalFormat, reinterpret_cast<const unsigned char*>(texture->pcData));
	}

	return textureID;
}

unsigned int TextureLoader::loadTextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);

	// Some exported models embed an absolute path from the original author's machine
	// (e.g. "C:/texture.jpg") instead of one relative to the model file. That path won't
	// exist here, but blindly joining it onto `directory` below would produce garbage like
	// "assets/models/Tree/C:/texture.jpg". Try it as-is first (rare but cheap), then fall
	// back to just its filename so the directory search below has something sane to join.
	if (std::filesystem::path(filename).is_absolute()) {
		unsigned int absoluteID = loadTexture(filename, gamma);
		if (absoluteID != 0) {
			return absoluteID;
		}
		filename = std::filesystem::path(filename).filename().string();
	}

	std::string fullPath = directory + '/' + filename;
	unsigned int textureID = loadTexture(fullPath, gamma);
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

std::vector<Texture> TextureLoader::loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type,
	const std::string& typeName,
	const std::string& directory)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// Assimp's embedded-texture convention: "*N" is an index into scene->mTextures,
		// not a path on disk (this is how glTF/GLB materials reference textures packed
		// into the model file itself rather than as separate image files). The index is
		// only unique within this scene, so the cache key below must include `directory`
		// to tell different models' "*0" apart (and, as a side benefit, stops two
		// different models that happen to share a relative filename like "diffuse.png"
		// from incorrectly sharing a cached texture).
		const aiTexture* embedded = (str.length > 0 && str.C_Str()[0] == '*')
			? scene->GetEmbeddedTexture(str.C_Str())
			: nullptr;
		std::string cacheKey = directory + "|" + str.C_Str();

		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++) {
			if (loadedTextures[j].path == cacheKey) {
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			unsigned int textureID = embedded
				? loadEmbeddedTexture(embedded)
				: loadTextureFromFile(str.C_Str(), directory);
			if (textureID != 0) {
				Texture texture = Texture(textureID, typeName, cacheKey);
				textures.push_back(texture);
				loadedTextures.push_back(texture);
			} else {
				std::cerr << "TextureLoader: Failed to load texture at path: " << str.C_Str() << std::endl;
			}
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


