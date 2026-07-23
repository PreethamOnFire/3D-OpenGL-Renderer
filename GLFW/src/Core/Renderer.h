#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "Camera.h"
#include "../Rendering/VertexArray.h"
#include "../Rendering/IndexBuffer.h"
#include "ShaderPipeline.h"
#include "../Rendering/Mesh.h"

class Renderer {
public:
	Renderer(GLFWwindow& window);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Camera& getCamera() const { return *camera; }

	void clear(float r = 0.2f, float g = 0.3f, float b = 0.3f, float a = 1.0f);
	void drawTriangles(Mesh& mesh, ShaderPipeline& pipeline);
	void setTime(float time) { currentTime = time; }
	void setDeltaTime(float dt) { deltaTime = dt; }
	void bindGlobalUniforms(ShaderPipeline& pipeline);

	int drawCallCount = 0;
	void resetDrawCalls() { drawCallCount = 0; }

private:
	float currentTime = 0.0f;
	float deltaTime = 0.0f;
	std::unique_ptr<Camera> camera;
	GLFWwindow* window;
};
