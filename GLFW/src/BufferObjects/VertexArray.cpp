#include <GL/glew.h>
#include "VertexArray.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

VertexArray::VertexArray() {
	glGenVertexArrays(1, &ID);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &ID);
}

void VertexArray::bind() const {
	glBindVertexArray(ID);
}

void VertexArray::unBind() const {
	glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(const VertexBuffer& vb, const std::vector<unsigned int>& layout) {
	bind();
	vb.bind();
	unsigned int offset = 0;
	unsigned int stride = 0;

	for (unsigned int size : layout) {
		stride += size * sizeof(float);
	}

	for (unsigned int i = 0; i < layout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, layout[i], GL_FLOAT, GL_FALSE, stride, (const void*)offset);
		offset += layout[i] * sizeof(float);
	}
}