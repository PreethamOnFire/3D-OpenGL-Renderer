#include "IndexBuffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const void* data, size_t size, bool mode, unsigned int count) {
	numIndicies = count;
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	if (mode) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}
	else {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &ID);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IndexBuffer::unBind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::getCount() {
	return numIndicies;
}