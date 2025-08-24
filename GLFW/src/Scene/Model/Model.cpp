#include <string>
#include <vector>
#include "../../BufferObjects/Mesh.h"
#include "../../Shader.h"
#include "../../Utility/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Renderer.h"
#include "Model.h"

Model::Model(const std::string& filePath, Shader& shader)
	: filePath(filePath), position(0.0f), rotation(0.0f), scale(1.0f) {
	auto result = ModelLoader::loadHierarchicalModel(filePath, shader);
	rootNode = std::move(result.first);
	materials = result.second;
	if (!rootNode) {
		std::cerr << "Failed to load model from " << filePath << std::endl;
		return;
	}
	this->shader = &shader;
}

Model::~Model() {
	for (auto* material : materials) {
		delete material;
	}
	materials.clear();
}

void Model::setPosition(const glm::vec3& position) {
	this->position = position;
	updateRootNodeTransform();
}

void Model::setRotation(const glm::vec3& rotation) {
	this->rotation = rotation;
	updateRootNodeTransform();
}

void Model::setScale(const glm::vec3& scale) {
	this->scale = scale;
	updateRootNodeTransform();
}

void Model::translate(const glm::vec3& deltaPosition) {
	position += deltaPosition;
	updateRootNodeTransform();
}

void Model::rotate(const glm::vec3& deltaRotation) {
	rotation += deltaRotation;
	updateRootNodeTransform();
}

void Model::updateRootNodeTransform() {
	if (rootNode) {
		rootNode->setPosition(position);
		rootNode->setRotation(rotation);
		rootNode->setScale(scale);
		rootNode->updateGlobalTransform();
	}
}

SceneNode* Model::getRootNode() const {
	return rootNode.get();
}

SceneNode* Model::findNode(const std::string& name) {
	if (!rootNode) return nullptr;

	if (rootNode->name == name) {
		return rootNode.get();
	}

	return rootNode->findChild(name);
}

bool Model::isLoaded() const {
	return rootNode != nullptr;
}

void Model::render(Renderer& renderer) {
	shader->use();
	if (rootNode) {
		rootNode->render(renderer, materials);
	}
}