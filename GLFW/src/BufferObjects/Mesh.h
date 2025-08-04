#pragma once
#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Shader.h"
#include <glm/mat4x4.hpp>

class Mesh {
private:
	const float* positions;
	const unsigned int* indicies;
public:
	Shader* shader;
	VertexArray* VAO;
	VertexBuffer* VBO;
	IndexBuffer* EBO;
	glm::mat4* modelMatrix;
	Mesh(const float* positions, size_t posSize, const unsigned int* indicies, size_t indSize, Shader &shader, const std::vector<unsigned int>& layout, unsigned int count);
	~Mesh();
};
