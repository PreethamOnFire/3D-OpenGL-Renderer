#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
class Camera {
public:
	float fov;
	float speed;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	Camera(GLFWwindow &window);
	void moveForward();
	void moveBackwards();
	void moveLeft();
	void moveRight();
	void panLeft();
	void panRight();
	void handleMouse();
	void updateProjectionMatrix(int width, int height);

private:
	void setViewMatrix();
};
