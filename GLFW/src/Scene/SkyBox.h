#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <memory>
#include "../Core/ShaderPipeline.h"
#include "../Rendering/VertexArray.h"
#include "../Rendering/VertexBuffer.h"
#include "../Core/Camera.h"

class SkyBox {
private:
	std::unique_ptr<VertexArray> VAO;
	std::unique_ptr<VertexBuffer> VBO;
	ShaderPipeline* pipeline;
	static const float skyboxVertices[];
	unsigned int textureID;
public:
	SkyBox(const std::vector<std::string>& faces, const std::string& directory, ShaderPipeline& pipeline);
	~SkyBox();
	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;

	void render(Camera& camera);
	void bind() const;
	void unbind() const;
};
