#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../BufferObjects/Mesh.h"
#include "../BufferObjects/Material.h"
#include "../Renderer.h"

class SceneNode {
public:
	std::string name;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::vector<Mesh*> meshes;
	std::vector<int> materialIndices;

	SceneNode* parent;
	std::vector<std::unique_ptr<SceneNode>> children;

	glm::mat4 localTransform;    
	glm::mat4 globalTransform;

	SceneNode(const std::string& nodeName = "Node");
	~SceneNode();

	void addChild(std::unique_ptr<SceneNode> child);
	SceneNode* findChild(const std::string name);

	void setPosition(const glm::vec3& pos);
	void setRotation(const glm::vec3& rot);
	void setScale(const glm::vec3& s);
	void rotate(const glm::vec3& deltaRotation);
	void translate(const glm::vec3& deltaPosition);

	void updateLocalTransform();
	void updateGlobalTransform(const glm::mat4& parentTransform = glm::mat4(1.0f));

	void render(Renderer& renderer, const std::vector<Material*>& materials);
};
