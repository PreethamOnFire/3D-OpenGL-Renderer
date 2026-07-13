#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../Rendering/Mesh.h"
#include "../Core/Shader.h"
#include "../Loaders/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Renderer.h"
#include "../Rendering/Material.h"
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
	void generateCube();
	void generateSphere(int segments = 32);
	void generatePlane(float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	static unsigned int nextID;
	ModelType modelType;
public:
	Model(const std::string& objectName, const std::string& filePath, Shader& shader);
	Model(const std::string& objectName, ModelType type, Shader& shader, float width = 1.0f, float height = 1.0f, int segments = 32);
	Model(const std::string& objectName, Mesh* customMesh, Shader& shader);
	~Model();
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	const std::string& getName() const { return name; }
	unsigned int getId() const { return id; }
	Shader* getShader() const { return shader; }
	Material* getMaterial(size_t index) const;

	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);

	void translate(const glm::vec3& deltaPosition);
	void rotate(const glm::vec3& deltaRotation);

	SceneNode* getRootNode() const;
	SceneNode* findNode(const std::string& name);

	bool isLoaded() const;

	void render(Renderer& renderer);

private:
	std::string name;
	Shader* shader;
	unsigned int id;
	std::vector<std::unique_ptr<Material>> materials;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
