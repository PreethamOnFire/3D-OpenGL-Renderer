#include "Camera.h"
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(GLFWwindow& window) {
	this->window = &window;
	fov = 100.0f;
	speed = 0.1f;
	yaw = -90.0f;
	pitch = 0.0f;
	firstMouse = true;
	eye = glm::vec3(0, 0, 0);
	at = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
	viewMatrix = glm::lookAt(eye, at, up);
	int width, height;
	glfwGetWindowSize(&window, &width, &height);
	projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
}

void Camera::setViewMatrix() {
	viewMatrix = glm::lookAt(eye, at, up);
}

void Camera::updateProjectionMatrix(float width, float height) {
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
}

void Camera::update(const InputManager& input, float deltaTime) {
	float velocity = speed * deltaTime;

	if (input.isKeyHeld(GLFW_KEY_W)) {
		glm::vec3 forward = glm::normalize(at - eye);
		eye += forward * velocity;
		at += forward * velocity;
	}
	if (input.isKeyHeld(GLFW_KEY_S)) {
		glm::vec3 forward = glm::normalize(at - eye);
		eye -= forward * velocity;
		at -= forward * velocity;
	}
	if (input.isKeyHeld(GLFW_KEY_A)) {
		glm::vec3 forward = glm::normalize(at - eye);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye -= right * velocity;
		at -= right * velocity;
	}
	if (input.isKeyHeld(GLFW_KEY_D)) {
		glm::vec3 forward = glm::normalize(at - eye);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye += right * velocity;
		at += right * velocity;
	}
	if (input.isKeyHeld(GLFW_KEY_Q)) {
		glm::vec3 forward = glm::normalize(at - eye);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye -= right * velocity;
		at -= right * velocity;
	}

	if (input.isKeyHeld(GLFW_KEY_E)) {
		glm::vec3 forward = glm::normalize(at - eye);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye += right * velocity;
		at += right * velocity;
	}

	// Hold shift to move faster
	if (input.isKeyHeld(GLFW_KEY_LEFT_SHIFT))
		speed = 10.0f;
	else
		speed = 5.0f;

	// Mouse look — only when cursor is captured
	if (input.isCursorCaptured()) {
		glm::vec2 delta = input.getMouseDelta();
		yaw += delta.x * sensitivity;
		pitch -= delta.y * sensitivity;
		pitch = glm::clamp(pitch, -89.0f, 89.0f);
		updateCameraVectors();
	}

	setViewMatrix();
}

void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 direction = glm::normalize(front);
	at = eye + direction;

	setViewMatrix();
}
