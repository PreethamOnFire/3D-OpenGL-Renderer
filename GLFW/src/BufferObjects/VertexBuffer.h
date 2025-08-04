#pragma once
class VertexBuffer {
private:
	unsigned int ID;

public:
	VertexBuffer(const void* data, size_t size, bool mode); //true for dynamic, false for static
	~VertexBuffer();

	void bind() const;
	void unBind() const;
};