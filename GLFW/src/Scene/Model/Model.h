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

class Model {
private:
	std::string filePath;
	std::unique_ptr<SceneNode> rootNode;
	void updateRootNodeTransform();
	Shader* shader;
public:
	std::vector<Material*> materials;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Model(const std::string& filePath, Shader& shader);
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
