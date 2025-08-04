#include "Mesh.h"
#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Shader.h"
#include <glm/mat4x4.hpp>

Mesh::Mesh(const float* positions, size_t posSize, const unsigned int* indices, size_t indSize, Shader& shader, const std::vector<unsigned int>& layout, unsigned int count) {
	this->shader = &shader;
	VAO = new VertexArray();
	VBO = new VertexBuffer(positions, posSize, false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = new IndexBuffer(indices, indSize, false, count);
	modelMatrix = new glm::mat4(1.0f);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Shader& shader) : positions(vertices), indices(indices) {
	this->shader = &shader;
	auto vertexData = Vertex::toFloatArray(vertices);
	auto layout = Vertex::getLayout();
	VAO = new VertexArray();
	VBO = new VertexBuffer(vertexData.data(), vertexData.size() * sizeof(float), false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = new IndexBuffer(indices.data(), indices.size() * sizeof(unsigned int), false, indices.size());
	modelMatrix = new glm::mat4(1.0f);

}

size_t Mesh::getVertexCount() {
	return positions.size();
}

size_t Mesh::getIndexCount() {
	return indices.size();
}

std::vector<Vertex>& Mesh::getPositions() {
	return positions;
}
std::vector<unsigned int>& Mesh::getIndicies() {
	return indices;
}

void Mesh::updateVertices(const std::vector<Vertex>& newVertices) {
	positions = newVertices;
	auto vertexData = Vertex::toFloatArray(newVertices);
	VBO->bind();
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
}

void Mesh::updateIndices(const std::vector<unsigned int>& newIndices) {
	indices = newIndices;
	EBO->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}

Mesh::~Mesh() {
	delete VAO;
	delete VBO;
	delete EBO;
	delete modelMatrix;
}