#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "Camera.h"
#include "BufferObjects/VertexArray.h"
#include "BufferObjects/IndexBuffer.h"
#include "Shader.h"
#include "BufferObjects/Mesh.h"

class Renderer {
private:

public:
	Camera* camera;
	GLFWwindow* window;
	Renderer(GLFWwindow& window);
	~Renderer();
	void Clear(float r = 0.2f, float g = 0.3f, float b = 0.3f, float a = 1.0f);
	void DrawTriangles(Mesh &mesh);
	void EnableDepthTest();
	void DisableDepthTest();
	void GetInput();
};
