#pragma once
#include <vector>
#include <string>
#include <memory>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <glm/mat4x4.hpp>
#include "Vertex.h"
#include "AABB.h"

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::string materialName);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	const std::string& getMaterialName() const { return materialName; }
	const glm::mat4& getModelMatrix() const { return modelMatrix; }
	void bind() const;

	void updateVertices(const std::vector<Vertex>& newVertices);
	void updateIndices(const std::vector<unsigned int>& newIndices);
	void updateModelMatrix(const glm::mat4& modelMatrix);
	size_t getVertexCount() const;
	size_t getIndexCount() const;
	const std::vector<Vertex>& getVertices() const;
	const std::vector<unsigned int>& getIndices() const;

	const AABB& getLocalBounds() const { return localBounds; }
	AABB getWorldBounds() const { return localBounds.transformed(modelMatrix); }

private:
	void recomputeLocalBounds();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string materialName;
	std::unique_ptr<VertexArray> VAO;
	std::unique_ptr<VertexBuffer> VBO;
	std::unique_ptr<IndexBuffer> EBO;
	glm::mat4 modelMatrix{1.0f};
	AABB localBounds;
};
