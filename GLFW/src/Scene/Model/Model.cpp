#include <string>
#include <vector>
#include "../../BufferObjects/Mesh.h"
#include "../../Shader.h"
#include "../../Utility/ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Renderer.h"
#include "Model.h"

Model::Model(const std::string& name, const std::string& filePath, Shader& shader)
	: filePath(filePath), position(0.0f), rotation(0.0f), scale(1.0f), shader(&shader), name(name), id(nextID++), modelType(ModelType::LOADED_MODEL) {
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

Model::Model(const std::string& objectName, ModelType type, Shader& shader)
	: id(nextID++), name(objectName), filePath(""), position(0.0f), rotation(0.0f), scale(1.0f), shader(&shader), modelType(type) {

	// Create root node
	rootNode = std::make_unique<SceneNode>(objectName);

	// Generate primitive based on type
	switch (type) {
	case ModelType::PRIMITIVE_CUBE:
		generateCube();
		break;
	case ModelType::PRIMITIVE_SPHERE:
		generateSphere();
		break;
	case ModelType::PRIMITIVE_PLANE:
		generatePlane();
		break;
	default:
		std::cerr << "Unknown primitive type!" << std::endl;
		break;
	}

	// Create default material
	Material* defaultMaterial = new Material();
	defaultMaterial->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	materials.push_back(defaultMaterial);
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

// New Way to add primitives to scene
void Model::generateCube() {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Cube vertices with normals and texture coordinates
	// Front face
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));

	// Back face
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));

	// Left face
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));

	// Right face
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	// Bottom face
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));

	// Top face
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
	vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));

	// Indices for cube faces
	indices = {
		0, 1, 2,   2, 3, 0,    // Front
		4, 5, 6,   6, 7, 4,    // Back
		8, 9, 10,  10, 11, 8,  // Left
		12, 13, 14, 14, 15, 12, // Right
		16, 17, 18, 18, 19, 16, // Bottom
		20, 21, 22, 22, 23, 20  // Top
	};

	// Create mesh and add to root node
	Mesh* cubeMesh = new Mesh(vertices, indices, *shader);
	rootNode->meshes.push_back(cubeMesh);
	rootNode->materialIndices.push_back(0);
}

void Model::generateSphere(int segments) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const float PI = 3.14159265359f;

	// Generate vertices
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
				glm::vec3(x * 0.5f, y * 0.5f, z * 0.5f),  // Position (radius 0.5)
				glm::vec3(x, y, z),                        // Normal
				glm::vec2(u, v)                            // Texture coords
			);
		}
	}

	for (int lat = 0; lat < segments; ++lat) {
		for (int lon = 0; lon < segments; ++lon) {
			int first = (lat * (segments + 1)) + lon;
			int second = first + segments + 1;

			// First triangle
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			// Second triangle
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	Mesh* sphereMesh = new Mesh(vertices, indices, *shader);
	rootNode->meshes.push_back(sphereMesh);
	rootNode->materialIndices.push_back(0);
}

// Will check this later for procedural plane generation + water shader
void Model::generatePlane(float width, float height, unsigned int segments) {
		std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float segmentWidth = width / segments;
	float segmentHeight = height / segments;
	// Generate vertices
	for (int y = 0; y <= segments; ++y) {
		for (int x = 0; x <= segments; ++x) {
			float posX = -halfWidth + x * segmentWidth;
			float posY = -halfHeight + y * segmentHeight;
			vertices.emplace_back(
				glm::vec3(posX, 0.0f, posY),          // Position
				glm::vec3(0.0f, 1.0f, 0.0f),          // Normal
				glm::vec2((float)x / segments, (float)y / segments) // Texture coords
			);
		}
	}
	// Generate indices
	for (int y = 0; y < segments; ++y) {
		for (int x = 0; x < segments; ++x) {
			int topLeft = y * (segments + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (y + 1) * (segments + 1) + x;
			int bottomRight = bottomLeft + 1;
			// First triangle
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			// Second triangle
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	Mesh* planeMesh = new Mesh(vertices, indices, *shader);
	rootNode->meshes.push_back(planeMesh);
	rootNode->materialIndices.push_back(0);
}