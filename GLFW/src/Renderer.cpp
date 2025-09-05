#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h";
#include "Camera.h";
#include "../src/BufferObjects/Mesh.h";
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(GLFWwindow& window) {
	camera = new Camera(window);
    this->window = &window;
    EnableDepthTest();
}
void Renderer::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EnableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::DisableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {
    delete camera;
}

void Renderer::GetInput() {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->moveForward();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->moveLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->moveRight();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->moveBackwards();
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera->panRight();
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera->panLeft();
    }
	camera->handleMouse();
}

void Renderer::DrawTriangles(Mesh &mesh) {
    mesh.shader->use();
    mesh.VAO->bind();
    mesh.EBO->bind();
    glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * *mesh.modelMatrix;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(*mesh.modelMatrix));
	mesh.shader->setMat4("MVP", MVP);
	mesh.shader->setMat4("modelMatrix", *mesh.modelMatrix);
	mesh.shader->setMat4("normalMatrix", normalMatrix);
    glDrawElements(GL_TRIANGLES, mesh.EBO->getCount(), GL_UNSIGNED_INT, nullptr);
}