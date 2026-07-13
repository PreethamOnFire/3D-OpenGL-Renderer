#include "Mesh.h"
#include <vector>
#include <memory>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Core/Shader.h"
#include <glm/mat4x4.hpp>

Mesh::Mesh(const float* positions, size_t posSize, const unsigned int* indices, size_t indSize, Shader& shader, const std::vector<unsigned int>& layout, unsigned int count) {
	this->shader = &shader;
	VAO = std::make_unique<VertexArray>();
	VBO = std::make_unique<VertexBuffer>(positions, posSize, false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = std::make_unique<IndexBuffer>(indices, indSize, false, count);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Shader& shader) : vertices(vertices), indices(indices) {
	this->shader = &shader;
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