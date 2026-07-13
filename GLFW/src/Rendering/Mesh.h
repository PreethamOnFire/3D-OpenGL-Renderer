#pragma once
#include <vector>
#include <memory>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Core/Shader.h"
#include <glm/mat4x4.hpp>
#include "Vertex.h"

class Mesh {
public:
	Mesh(const float* positions, size_t posSize, const unsigned int* indices, size_t indSize, Shader& shader, const std::vector<unsigned int>& layout, unsigned int count);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Shader& shader);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Shader* getShader() const { return shader; }
	const glm::mat4& getModelMatrix() const { return modelMatrix; }
	void bind() const;

	void updateVertices(const std::vector<Vertex>& newVertices);
	void updateIndices(const std::vector<unsigned int>& newIndices);
	void updateModelMatrix(const glm::mat4& modelMatrix);
	size_t getVertexCount() const;
	size_t getIndexCount() const;
	const std::vector<Vertex>& getVertices() const;
	const std::vector<unsigned int>& getIndices() const;

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Shader* shader;
	std::unique_ptr<VertexArray> VAO;
	std::unique_ptr<VertexBuffer> VBO;
	std::unique_ptr<IndexBuffer> EBO;
	glm::mat4 modelMatrix{1.0f};
};
