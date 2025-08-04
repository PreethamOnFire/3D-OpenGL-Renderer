#include <GL/glew.h>
#include "VertexBuffer.h"
#include <iostream>

VertexBuffer::VertexBuffer(const void* data, size_t size, bool mode) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	if (mode) {
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}
	else {
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &ID);
}

void VertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBuffer::unBind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}