#pragma once
class VertexBuffer {
private:
	unsigned int ID;

public:
	VertexBuffer(const void* data, size_t size, bool mode);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	void bind() const;
	void unbind() const;
};