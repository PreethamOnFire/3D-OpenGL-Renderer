#pragma once
#include "VertexBuffer.h"
#include <vector>

class VertexArray {
private:
	unsigned int ID;

public:
	VertexArray(); //true for dynamic, false for static
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	void bind() const;
	void unbind() const;
	void addVertexBuffer(const VertexBuffer& vb, const std::vector<unsigned int>& layout);
};
