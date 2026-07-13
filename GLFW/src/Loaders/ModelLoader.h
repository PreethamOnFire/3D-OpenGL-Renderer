#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../Rendering/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "../Rendering/Material.h"
#include "../Scene/SceneNode.h"

class ModelLoader {
public:
	static std::pair<std::unique_ptr<SceneNode>, std::vector<std::unique_ptr<Material>>>
		loadHierarchicalModel(const std::string& filePath, Shader& shader);
private:
	static std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, Shader& shader);
	static std::unique_ptr<SceneNode> processNode(aiNode* node, const aiScene* scene, Shader& shader);
	static std::vector<std::unique_ptr<Material>> loadMaterials(const aiScene* scene, const std::string& directory);
	static glm::vec3 quatToEuler(const aiQuaternion& q);
};
