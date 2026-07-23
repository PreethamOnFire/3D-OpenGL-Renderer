#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Rendering/Mesh.h"
#include "../Rendering/MaterialLibrary.h"
#include "../Core/Renderer.h"
#include "SceneNode.h"

SceneNode::SceneNode(const std::string& nodeName)
    : name(nodeName), position(0.0f), rotation(0.0f), scale(1.0f),
    parent(nullptr), localTransform(1.0f), globalTransform(1.0f) {
    updateLocalTransform();
}

void SceneNode::addMesh(std::unique_ptr<Mesh> mesh) {
    meshes.push_back(std::move(mesh));
}

SceneNode::~SceneNode() = default;

void SceneNode::addChild(std::unique_ptr<SceneNode> child) {
    child->parent = this;
    children.push_back(std::move(child));
}

SceneNode* SceneNode::findChild(const std::string& name) {
    for (auto& child : children) {
        if (child->name == name) {
            return child.get();
        }
    }

    for (auto& child : children) {
        SceneNode* found = child->findChild(name);
        if (found) {
            return found;
		}
    }

    return nullptr;
}

void SceneNode::setPosition(const glm::vec3& pos) {
    position = pos;
    updateLocalTransform();
}

void SceneNode::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateLocalTransform();
}

void SceneNode::setScale(const glm::vec3& s) {
    scale = s;
    updateLocalTransform();
}

void SceneNode::rotate(const glm::vec3& deltaRotation) {
    rotation += deltaRotation;
    updateLocalTransform();
}

void SceneNode::translate(const glm::vec3& deltaPosition) {
    position += deltaPosition;
    updateLocalTransform();
}

void SceneNode::updateLocalTransform() {
	localTransform = glm::mat4(1.0f);
	localTransform = glm::translate(localTransform, position);
	localTransform = glm::rotate(localTransform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	localTransform = glm::rotate(localTransform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	localTransform = glm::rotate(localTransform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	localTransform = glm::scale(localTransform, scale);
}

void SceneNode::updateGlobalTransform(const glm::mat4& parentTransform) {
    globalTransform = parentTransform * localTransform;
    for (auto& mesh : meshes) {
        if (mesh) {
            mesh->updateModelMatrix(globalTransform);
        }
    }
    for (auto& child : children) {
        child->updateGlobalTransform(globalTransform);
    }
}

void SceneNode::render(Renderer& renderer, const MaterialLibrary& materials) {
    for (auto& mesh : meshes) {
        if (!mesh) continue;
        Material& material = materials.getOrDefault(mesh->getMaterialName());
        material.bind();
        renderer.drawTriangles(*mesh, *material.getPipeline());
        material.unbind();
    }
    for (auto& child : children) {
		child->render(renderer, materials);
    }
}

