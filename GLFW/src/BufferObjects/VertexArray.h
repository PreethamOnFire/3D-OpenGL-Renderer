#pragma once
#include "VertexBuffer.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

class VertexArray {
private:
	unsigned int ID;

public:
	VertexArray(); //true for dynamic, false for static
	~VertexArray();

	void bind() const;
	void unBind() const;
	void addVertexBuffer(const VertexBuffer& vb, const std::vector<unsigned int>& layout);
};
