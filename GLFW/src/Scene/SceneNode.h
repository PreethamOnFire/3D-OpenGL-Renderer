#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Rendering/Mesh.h"
#include "../Rendering/MaterialLibrary.h"
#include "../Core/Renderer.h"

class SceneNode {
public:
	SceneNode(const std::string& nodeName = "Node");
	~SceneNode();

	const std::string& getName() const { return name; }
	bool hasMeshes() const { return !meshes.empty(); }
	size_t getMeshCount() const { return meshes.size(); }
	void addMesh(std::unique_ptr<Mesh> mesh);

	void addChild(std::unique_ptr<SceneNode> child);
	SceneNode* findChild(const std::string& name);

	void setPosition(const glm::vec3& pos);
	void setRotation(const glm::vec3& rot);
	void setScale(const glm::vec3& s);
	void rotate(const glm::vec3& deltaRotation);
	void translate(const glm::vec3& deltaPosition);

	void updateLocalTransform();
	void updateGlobalTransform(const glm::mat4& parentTransform = glm::mat4(1.0f));

	void render(Renderer& renderer, const MaterialLibrary& materials);

private:
	std::string name;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::vector<std::unique_ptr<Mesh>> meshes;

	SceneNode* parent;
	std::vector<std::unique_ptr<SceneNode>> children;

	glm::mat4 localTransform;
	glm::mat4 globalTransform;
};
