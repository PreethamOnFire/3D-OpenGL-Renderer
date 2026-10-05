#include "Mesh.h"
#include <vector>
#include <memory>
#include <GL/glew.h>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Core/ShaderPipeline.h"
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::string materialName) : vertices(vertices), indices(indices), materialName(std::move(materialName)) {
	auto vertexData = Vertex::toFloatArray(vertices);
	auto layout = Vertex::getLayout();
	VAO = std::make_unique<VertexArray>();
	VBO = std::make_unique<VertexBuffer>(vertexData.data(), vertexData.size() * sizeof(float), false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = std::make_unique<IndexBuffer>(indices.data(), indices.size() * sizeof(unsigned int), false, indices.size());
	recomputeLocalBounds();
}

void Mesh::recomputeLocalBounds() {
	localBounds = AABB();
	for (const auto& vertex : vertices) {
		localBounds.expand(vertex.position);
	}
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
	recomputeLocalBounds();
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

void Mesh::render(ShaderPipeline& pipeline, const glm::mat4& viewProj) const {
	bind();
	glm::mat4 MVP = viewProj * modelMatrix;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	pipeline.setMat4("MVP", MVP);
	pipeline.setMat4("modelMatrix", modelMatrix);
	pipeline.setMat4("normalMatrix", normalMatrix);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
}

Mesh::~Mesh() = default;