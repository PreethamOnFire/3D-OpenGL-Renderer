#pragma once
#include <string>
#include <vector>
#include "../BufferObjects/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "../BufferObjects/Material.h"
#include "../Scene/SceneNode.h"

class ModelLoader {
public:
	static Mesh* loadModel(const std::string& filePath, Shader &shader);
	static std::pair<std::unique_ptr<SceneNode>, std::vector<Material*>>
		loadHierarchicalModel(const std::string& filePath, Shader& shader);
private:
	static Mesh* processMesh(aiMesh* mesh, const aiScene* scene, Shader& shader);
	static std::unique_ptr<SceneNode> processNode(aiNode* node, const aiScene* scene, Shader& shader);
	static std::vector<Material*> loadMaterials(const aiScene* scene);
	static glm::vec3 quatToEuler(const aiQuaternion& q);
};
