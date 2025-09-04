#pragma once
#include <string>
#include <vector>
#include "../../BufferObjects/Mesh.h"
#include "../../Shader.h"
#include "../../Utility/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Renderer.h"
#include "../../BufferObjects/Material.h"
#include "../SceneNode.h"

enum class ModelType {
	LOADED_MODEL,    // From file (OBJ, etc.)
	PRIMITIVE_CUBE,  // Generated cube
	PRIMITIVE_SPHERE,// Generated sphere
	PRIMITIVE_PLANE, // Generated plane
	CUSTOM_MESH     // User-provided mesh
};

class Model {
private:
	std::string filePath;
	std::unique_ptr<SceneNode> rootNode;
	void updateRootNodeTransform();
	void generateCube();
	void generateSphere(int segments = 32);
	void generatePlane(float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	unsigned int nextID;
	ModelType modelType;
public:
	std::string name;
	Shader* shader;
	unsigned int id;
	std::vector<Material*> materials;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Model(const std::string& objectName, const std::string& filePath, Shader& shader);
	Model(const std::string& objectName, ModelType type, Shader& shader);               // Primitives
	Model(const std::string& objectName, Mesh* customMesh, Shader& shader);             // Custom mesh
	~Model();

	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);

	void translate(const glm::vec3& deltaPosition);
	void rotate(const glm::vec3& deltaRotation);

	SceneNode* getRootNode() const;
	SceneNode* findNode(const std::string& name);

	bool isLoaded() const;

	void render(Renderer& renderer);
};
