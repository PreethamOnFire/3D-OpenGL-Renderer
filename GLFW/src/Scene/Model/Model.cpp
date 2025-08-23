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
	mesh = ModelLoader::loadModel(filePath, shader);
	if (!mesh) {
		throw std::runtime_error("Failed to load model from " + filePath);
	}
}

Model::~Model() {
	for (auto* mesh : meshes) {
		delete mesh;
	}
	for (auto* material : materials) {
		delete material;
	}
}

void Model::setPosition(const glm::vec3& position) {
	this->position = position;
	updateModelMatrix();
}

void Model::setRotation(const glm::vec3& rotation) {
	this->rotation = rotation;
	updateModelMatrix();
}

void Model::setScale(const glm::vec3& scale) {
	this->scale = scale;
	updateModelMatrix();
}

void Model::updateModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale);
	mesh->updateModelMatrix(model);
}

void Model::render(Renderer& renderer) {
	if (!mesh) {
		throw std::runtime_error("Mesh is not loaded for model: " + filePath);
	}
	renderer.DrawTriangles(*mesh);
}