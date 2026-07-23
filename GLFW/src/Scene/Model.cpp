#include <string>
#include <vector>
#include <iostream>
#include "../Rendering/Mesh.h"
#include "../Loaders/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Renderer.h"
#include "Model.h"

unsigned int Model::nextID = 0;

Model::Model(const std::string& name, const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials)
	: filePath(filePath), position(0.0f), rotation(0.0f), scale(1.0f), name(name), id(nextID++), modelType(ModelType::LOADED_MODEL) {
	rootNode = ModelLoader::loadHierarchicalModel(filePath, pipeline, materials, name);
	if (!rootNode) {
		std::cerr << "Failed to load model from " << filePath << std::endl;
		return;
	}

	if (!rootNode->hasMeshes()) {
		std::cerr << "WARNING: Root node has no meshes!" << std::endl;
	} else {
		std::cout << "Root node has " << rootNode->getMeshCount() << " meshes" << std::endl;
	}
}

Model::Model(const std::string& objectName, ModelType type, ShaderPipeline& pipeline, MaterialLibrary& materials, float width, float height, int segments)
	: id(nextID++), name(objectName), filePath(""), position(0.0f), rotation(0.0f), scale(1.0f), modelType(type) {

	rootNode = std::make_unique<SceneNode>(objectName);

	std::string materialName = objectName + "_default";
	Material& defaultMaterial = materials.create(materialName, pipeline);
	defaultMaterial.setVec3("diffuse", glm::vec3(0.8f, 0.8f, 0.8f));

	switch (type) {
	case ModelType::PRIMITIVE_CUBE:
		generateCube(materialName);
		break;
	case ModelType::PRIMITIVE_SPHERE:
		generateSphere(materialName, segments);
		break;
	case ModelType::PRIMITIVE_PLANE:
		generatePlane(materialName, width, height, static_cast<unsigned int>(segments));
		break;
	default:
		std::cerr << "Unknown primitive type!" << std::endl;
		break;
	}
}

Model::~Model() = default;

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
	if (rootNode->getName() == name) return rootNode.get();
	return rootNode->findChild(name);
}

bool Model::isLoaded() const {
	return rootNode != nullptr;
}

void Model::render(Renderer& renderer, const MaterialLibrary& materials) {
	if (!rootNode) return;
	rootNode->render(renderer, materials);
}

void Model::generateCube(const std::string& materialName) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));

	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));

	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));

	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));

	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));

	indices = {
		0, 1, 2,   2, 3, 0,
		4, 5, 6,   6, 7, 4,
		8, 9, 10,  10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	rootNode->addMesh(std::make_unique<Mesh>(vertices, indices, materialName));
}

void Model::generateSphere(const std::string& materialName, int segments) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const float PI = 3.14159265359f;

	for (int lat = 0; lat <= segments; ++lat) {
		float theta = lat * PI / segments;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segments; ++lon) {
			float phi = lon * 2 * PI / segments;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = cosPhi * sinTheta;
			float y = cosTheta;
			float z = sinPhi * sinTheta;

			float u = 1.0f - (float)lon / segments;
			float v = 1.0f - (float)lat / segments;

			vertices.emplace_back(
				glm::vec3(x * 0.5f, y * 0.5f, z * 0.5f),
				glm::vec3(x, y, z),
				glm::vec2(u, v)
			);
		}
	}

	for (int lat = 0; lat < segments; ++lat) {
		for (int lon = 0; lon < segments; ++lon) {
			int first = (lat * (segments + 1)) + lon;
			int second = first + segments + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	rootNode->addMesh(std::make_unique<Mesh>(vertices, indices, materialName));
}

void Model::generatePlane(const std::string& materialName, float width, float height, unsigned int segments) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float segmentWidth = width / segments;
	float segmentHeight = height / segments;
	for (int y = 0; y <= (int)segments; ++y) {
		for (int x = 0; x <= (int)segments; ++x) {
			float posX = -halfWidth + x * segmentWidth;
			float posZ = -halfHeight + y * segmentHeight;
			vertices.emplace_back(
				glm::vec3(posX, 0.0f, posZ),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec2((float)x / segments, (float)y / segments)
			);
		}
	}
	for (int y = 0; y < (int)segments; ++y) {
		for (int x = 0; x < (int)segments; ++x) {
			int topLeft = y * (segments + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (y + 1) * (segments + 1) + x;
			int bottomRight = bottomLeft + 1;
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	rootNode->addMesh(std::make_unique<Mesh>(vertices, indices, materialName));
}
