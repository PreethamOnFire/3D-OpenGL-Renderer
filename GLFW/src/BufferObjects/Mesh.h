#pragma once
#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Shader.h"
#include <glm/mat4x4.hpp>
#include "Vertex.h"

class Mesh {
private:
	std::vector<Vertex> positions;
	std::vector<unsigned int> indices;
public:
	Shader* shader;
	VertexArray* VAO;
	VertexBuffer* VBO;
	IndexBuffer* EBO;
	glm::mat4* modelMatrix;
	Mesh(const float* positions, size_t posSize, const unsigned int* indicies, size_t indSize, Shader &shader, const std::vector<unsigned int>& layout, unsigned int count);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Shader& shader);
	void updateVertices(const std::vector<Vertex>& newVertices);
	void updateIndices(const std::vector<unsigned int>& newIndices);
	size_t getVertexCount();
	size_t getIndexCount();
	std::vector<Vertex>& getPositions();
	std::vector<unsigned int>& getIndicies();
	~Mesh();
};
