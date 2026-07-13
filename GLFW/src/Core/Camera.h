#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "InputManager.h"
class Camera {
public:
	Camera(GLFWwindow& window);

	const glm::mat4& getViewMatrix() const { return viewMatrix; }
	const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
	const glm::vec3& getEye() const { return eye; }
	void updateProjectionMatrix(float width, float height);
	void update(const InputManager& input, float deltaTime);

private:
	float fov;
	float speed;
	float yaw;
	float pitch;
	float sensitivity = 0.1f;
	bool firstMouse;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	double lastX, lastY;
	GLFWwindow* window;
	void setViewMatrix();
	void updateCameraVectors();
};
