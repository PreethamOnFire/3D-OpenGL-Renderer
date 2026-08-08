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
	const float getYaw() const { return yaw; }
	const float getPitch() const { return pitch; }
	float getSpeed() const { return speed; }
	float getBaseSpeed() const { return baseSpeed; }
	void setBaseSpeed(float newBaseSpeed) { baseSpeed = newBaseSpeed; }
	float getSensitivity() const { return sensitivity; }
	void setSensitivity(float newSensitivity) { sensitivity = newSensitivity; }
	float getFov() const { return fov; } // stored in degrees
	void setFov(float degrees);
	void setEye(const glm::vec3& newEye); // teleport — keeps current look direction
	void updateProjectionMatrix(float width, float height);
	void update(const InputManager& input, float deltaTime);

private:
	float fov;
	float speed;
	float baseSpeed = 5.0f; // walk speed before the Shift-sprint multiplier
	float yaw;
	float pitch;
	float sensitivity = 0.1f;
	bool firstMouse;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	float lastWidth = 0.0f, lastHeight = 0.0f; // cached for setFov() to recompute the projection matrix
	double lastX, lastY;
	GLFWwindow* window;
	void setViewMatrix();
	void updateCameraVectors();
};
