#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <assimp/scene.h>
#include "../Rendering/Material.h"

class TextureLoader {
private:
	static unsigned int loadTextureFromFile(const char* path, const std::string& directory, bool gamma = false);
	static unsigned int loadEmbeddedTexture(const aiTexture* texture, bool gamma = false);
	static bool channelsToFormat(int channels, bool gamma, GLenum& format, GLenum& internalFormat);
	static void uploadTexture2D(unsigned int textureID, int width, int height, GLenum format, GLenum internalFormat, const unsigned char* data);
	static std::vector<Texture> loadedTextures;
public:
	static unsigned int loadTexture(const std::string& path, bool gamma = false);
	// `scene` is needed to resolve embedded textures (glTF/GLB materials can reference
	// scene->mTextures by index, e.g. "*0", instead of a path on disk).
	static std::vector<Texture> loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type,
		const std::string& typeName,
		const std::string& directory);
	static unsigned int loadCubeMap(const std::vector<std::string>& faces, const std::string& directory, bool gamma = false);
	static void clearCache();
};
