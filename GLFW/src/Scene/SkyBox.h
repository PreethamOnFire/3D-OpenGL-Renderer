#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include "../Shader.h"
#include "../BufferObjects/VertexArray.h"
#include "../BufferObjects/VertexBuffer.h"
#include "../Camera.h"

class SkyBox {
private:
	VertexArray* VAO;
	VertexBuffer* VBO;
	Shader* shader;
	static const float skyboxVertices[];
	unsigned int textureID;
public:
	SkyBox(const std::vector<std::string>& faces, std::string& directory, Shader& shader);
	~SkyBox();

	void render(Camera& camera);
	void bind() const;
	void unbind() const;
};
