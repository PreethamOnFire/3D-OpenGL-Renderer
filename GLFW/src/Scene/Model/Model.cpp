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
	: filePath(filePath), position(0.0f), rotation(0.0f), scale(1.0f), shader(&shader) {
	auto result = ModelLoader::loadHierarchicalModel(filePath, shader);
	rootNode = std::move(result.first);
	materials = result.second;
	if (!rootNode) {
		std::cerr << "Failed to load model from " << filePath << std::endl;
		return;
	}

	for (size_t i = 0; i < materials.size(); ++i) {
		if (!materials[i]) {
			std::cerr << "ERROR: Material " << i << " is null!" << std::endl;
		}
		else {
			std::cout << "Material " << i << " has " << materials[i]->getTextures().size() << " textures" << std::endl;
		}
	}

	// Debug: Check if rootNode has valid meshes
	if (rootNode->meshes.empty()) {
		std::cerr << "WARNING: Root node has no meshes!" << std::endl;
	}
	else {
		std::cout << "Root node has " << rootNode->meshes.size() << " meshes" << std::endl;
		for (size_t i = 0; i < rootNode->meshes.size(); ++i) {
			if (!rootNode->meshes[i]) {
				std::cerr << "ERROR: Mesh " << i << " is null!" << std::endl;
			}
		}
	}
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
	if (!rootNode) return;
	rootNode->setPosition(position);
	rootNode->setRotation(rotation);
	rootNode->setScale(scale);
	rootNode->updateGlobalTransform();
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
	if (!rootNode || !shader) return;
	shader->use();
	rootNode->render(renderer, materials);
}