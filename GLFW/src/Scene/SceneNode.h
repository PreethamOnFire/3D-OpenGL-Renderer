#pragma once
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Rendering/Mesh.h"
#include "../Rendering/MaterialLibrary.h"
#include "../Rendering/AABB.h"
#include "../Rendering/RenderQueue.h"
#include "../Rendering/Frustum.h"
#include "../Core/Renderer.h"
#include "SceneStats.h"

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
	const std::vector<std::unique_ptr<SceneNode>>& getChildren() const { return children; }

	void setPosition(const glm::vec3& pos);
	void setRotation(const glm::vec3& rot);
	void setScale(const glm::vec3& s);
	void rotate(const glm::vec3& deltaRotation);
	void translate(const glm::vec3& deltaPosition);

	void updateLocalTransform();
	void updateGlobalTransform(const glm::mat4& parentTransform = glm::mat4(1.0f));

	void collectRenderCommands(RenderQueue& queue, const MaterialLibrary& materials, const glm::vec3& camEye, const Frustum& frustum) const;

	void setCastsShadow(bool value);
	void collectShadowCasters(std::vector<Mesh*>& out) const;

	SceneStats getStats() const;
	void collectMaterialNames(std::set<std::string>& out) const;
	AABB getWorldBounds() const;

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
