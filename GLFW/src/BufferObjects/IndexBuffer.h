#pragma once
class IndexBuffer {
private:
	unsigned int ID;
	unsigned int numIndicies;
public:
	IndexBuffer(const void* data, size_t size, bool mode, unsigned int count);
	~IndexBuffer();
	unsigned int getCount();
	void bind() const;
	void unBind() const;
};
