#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../Rendering/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "../Rendering/MaterialLibrary.h"
#include "../Scene/SceneNode.h"

class ModelLoader {
public:
	static std::unique_ptr<SceneNode> loadHierarchicalModel(const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials, const std::string& modelName);
private:
	static std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelName);
	static std::unique_ptr<SceneNode> processNode(aiNode* node, const aiScene* scene, const std::string& modelName);
	static void loadMaterials(const aiScene* scene, const std::string& directory, ShaderPipeline& pipeline, MaterialLibrary& materials, const std::string& modelName);
	static glm::vec3 quatToEuler(const aiQuaternion& q);
};
