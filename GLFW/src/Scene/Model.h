#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../Rendering/Mesh.h"
#include "../Loaders/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Renderer.h"
#include "../Rendering/MaterialLibrary.h"
#include "SceneNode.h"

enum class ModelType {
	LOADED_MODEL,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_PLANE,
	CUSTOM_MESH
};

class Model {
private:
	std::string filePath;
	std::unique_ptr<SceneNode> rootNode;
	void updateRootNodeTransform();
	void generateCube(const std::string& materialName);
	void generateSphere(const std::string& materialName, int segments = 32);
	void generatePlane(const std::string& materialName, float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	static unsigned int nextID;
	ModelType modelType;
public:
	Model(const std::string& objectName, const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials);
	Model(const std::string& objectName, ModelType type, ShaderPipeline& pipeline, MaterialLibrary& materials, float width = 1.0f, float height = 1.0f, int segments = 32);
	~Model();
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	const std::string& getName() const { return name; }
	unsigned int getId() const { return id; }

	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);

	void translate(const glm::vec3& deltaPosition);
	void rotate(const glm::vec3& deltaRotation);

	SceneNode* getRootNode() const;
	SceneNode* findNode(const std::string& name);

	bool isLoaded() const;

	void render(Renderer& renderer, const MaterialLibrary& materials);

private:
	std::string name;
	unsigned int id;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
