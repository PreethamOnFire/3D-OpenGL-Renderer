#include "Mesh.h"
#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Shader.h"
#include <glm/mat4x4.hpp>

Mesh::Mesh(const float* positions, size_t posSize, const unsigned int* indicies, size_t indSize, Shader& shader, const std::vector<unsigned int>& layout, unsigned int count) {
	this->positions = positions;
	this->indicies = indicies;
	this->shader = &shader;
	VAO = new VertexArray();
	VBO = new VertexBuffer(this->positions, posSize, false);
	VAO->addVertexBuffer(*VBO, layout);
	EBO = new IndexBuffer(this->indicies, indSize, false, count);
	modelMatrix = new glm::mat4(1.0f);
}

Mesh::~Mesh() {
	delete VAO;
	delete VBO;
	delete EBO;
	delete modelMatrix;
}