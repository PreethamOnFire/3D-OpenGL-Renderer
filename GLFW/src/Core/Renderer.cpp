#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Camera.h"
#include "../Rendering/Mesh.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(GLFWwindow& window) {
	camera = std::make_unique<Camera>(window);
    this->window = &window;
}
void Renderer::clear() {
    resetDrawCalls();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setWireframe(bool enabled) {
    wireframe = enabled;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

Renderer::~Renderer() = default;

void Renderer::bindGlobalUniforms(ShaderPipeline& pipeline) {
    pipeline.use();
    pipeline.setFloat("time", currentTime);
    pipeline.setFloat("deltaTime", deltaTime);
}

void Renderer::drawTriangles(Mesh& mesh, ShaderPipeline& pipeline) {
    glm::mat4 viewProj = camera->getProjectionMatrix() * camera->getViewMatrix();
    mesh.render(pipeline, viewProj);
    drawCallCount++;
}

void Renderer::restoreViewport() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}
