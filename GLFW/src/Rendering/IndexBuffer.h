#pragma once
class IndexBuffer {
private:
	unsigned int ID;
	unsigned int numIndices;
public:
	IndexBuffer(const void* data, size_t size, bool mode, unsigned int count);
	~IndexBuffer();
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	unsigned int getCount() const;
	void bind() const;
	void unbind() const;
};
