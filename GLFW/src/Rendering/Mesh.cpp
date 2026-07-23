#include "Mesh.h"
#include <vector>
#include <memory>
#include <GL/glew.h>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <glm/mat4x4.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::string materialName) : vertices(vertices), indices(indices), materialName(std::move(materialName)) {
	auto vertexData = Vertex::toFloatArray(vertices);
	auto layout = Vertex::getLayout();
	VAO = std::make_unique<VertexArray>();
	VBO = std::make_unique<VertexBuffer>(vertexData.data(), vertexData.size() * sizeof(float), false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = std::make_unique<IndexBuffer>(indices.data(), indices.size() * sizeof(unsigned int), false, indices.size());
}

size_t Mesh::getVertexCount() const {
	return vertices.size();
}

size_t Mesh::getIndexCount() const {
	return indices.size();
}

const std::vector<Vertex>& Mesh::getVertices() const {
	return vertices;
}

const std::vector<unsigned int>& Mesh::getIndices() const {
	return indices;
}

void Mesh::updateVertices(const std::vector<Vertex>& newVertices) {
	vertices = newVertices;
	auto vertexData = Vertex::toFloatArray(newVertices);
	VBO->bind();
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
}

void Mesh::updateIndices(const std::vector<unsigned int>& newIndices) {
	indices = newIndices;
	EBO->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}

void Mesh::updateModelMatrix(const glm::mat4& modelMatrix) {
	this->modelMatrix = modelMatrix;
}

void Mesh::bind() const {
	VAO->bind();
	EBO->bind();
}

Mesh::~Mesh() = default;