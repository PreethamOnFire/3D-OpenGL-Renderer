#include "Camera.h"
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(GLFWwindow& window) {
	fov = 100.0f;
	speed = 0.01f;
	eye = glm::vec3(0, 0, 0);
	at = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	viewMatrix = glm::lookAt(eye, at, up);
	int width, height;
	glfwGetWindowSize(&window, &width, &height);
	projectionMatrix = glm::perspective(fov, float(width / height), 0.1f, 1000.0f);
}

void Camera::setViewMatrix() {
	viewMatrix = glm::lookAt(eye, at, up);
}

void Camera::updateProjectionMatrix(float width, float height) {
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	projectionMatrix = glm::perspective(fov, aspectRatio, 0.1f, 1000.0f);
}

void Camera::moveForward() {
	glm::vec3 f = at;
	f -= eye;
	f = glm::normalize(f);
	f *= speed;
	eye += f;
	at += f;
	setViewMatrix();
}

void Camera::moveBackwards() {
	glm::vec3 f = at;
	f -= eye;
	f = glm::normalize(f);
	f *= speed;
	eye -= f;
	at -= f;
	setViewMatrix();
}

void Camera::moveLeft() {
	glm::vec3 f = at;
	f -= eye;
	glm::vec3 s = glm::cross(f, up);
	s *= speed;
	eye += s;
	at += s;
	setViewMatrix();
}

void Camera::moveRight() {
	glm::vec3 f = at;
	f -= eye;
	glm::vec3 s = glm::cross(up, f);
	s *= speed;
	eye += s;
	at += s;
	setViewMatrix();
}

void Camera::panRight() {
	glm::vec3 f = at;
	f -= eye;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), up);
	glm::vec3 fPrime = rotationMatrix * glm::vec4(f, 0.0f);
	f = eye;
	f += fPrime;
	at = f;
	setViewMatrix();
}

void Camera::panLeft() {
	glm::vec3 f = at;
	f -= eye;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), up);
	glm::vec3 fPrime = rotationMatrix * glm::vec4(f, 0.0f);
	f = eye;
	f += fPrime;
	at = f;
	setViewMatrix();
}