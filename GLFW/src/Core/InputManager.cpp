#include "InputManager.h"

void InputManager::update(GLFWwindow* window) {
	previousKeys = currentKeys;
	previousButtons = currentButtons;
	currentKeys.clear();
	currentButtons.clear();

	for (int key : TRACKED_KEYS) {
		if (glfwGetKey(window, key) == GLFW_PRESS)
			currentKeys.insert(key);
	}

	for (int btn : TRACKED_BUTTONS) {
		if (glfwGetMouseButton(window, btn) == GLFW_PRESS)
			currentButtons.insert(btn);
	}

	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	glm::vec2 newPos = { (float)mx, (float)my };

	if (firstFrame) {
		lastMousePos = newPos;
		firstFrame = false;
	}

	mouseDelta = newPos - lastMousePos;
	lastMousePos = newPos;
	mousePos = newPos;
}

// Keyboard
bool InputManager::isKeyHeld(int glfwKey) const {
	if (currentKeys.count(glfwKey) > 0) {
		return true;
	}
}

bool InputManager::isKeyJustPressed(int glfwKey) const {
	return currentKeys.count(glfwKey) > 0 && previousKeys.count(glfwKey) == 0;
}
bool InputManager::isKeyJustReleased(int glfwKey) const {
	return currentKeys.count(glfwKey) > 0 && previousKeys.count(glfwKey) > 0;
}

// Mouse buttons
bool InputManager::isMouseButtonHeld(int button) const {
	if (currentButtons.count(button) > 0) {
		return true;
	}
}
bool InputManager::isMouseButtonJustPressed(int button) const {
	return currentButtons.count(button) > 0 && previousButtons.count(button) == 0;
}

// Cursor control — call once during init
void InputManager::captureCursor(GLFWwindow* window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	cursorCaptured = true;
	firstFrame = true;
}
void InputManager::releaseCursor(GLFWwindow* window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cursorCaptured = false;
}