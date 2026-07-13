#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_set>

class InputManager {
public:
    // Call once per frame BEFORE any system reads input
    void update(GLFWwindow* window);

    // Keyboard
    bool isKeyHeld(int glfwKey) const;       // held this frame
    bool isKeyJustPressed(int glfwKey) const; // pressed THIS frame only
    bool isKeyJustReleased(int glfwKey) const;// released THIS frame only

    // Mouse position & movement
    glm::vec2 getMousePos() const { return mousePos; }
    glm::vec2 getMouseDelta() const { return mouseDelta; }

    // Mouse buttons
    bool isMouseButtonHeld(int button) const;
    bool isMouseButtonJustPressed(int button) const;

    // Cursor control — call once during init
    void captureCursor(GLFWwindow* window);
    void releaseCursor(GLFWwindow* window);
    bool isCursorCaptured() const { return cursorCaptured; }

private:
    std::unordered_set<int> currentKeys;
    std::unordered_set<int> previousKeys;
    std::unordered_set<int> currentButtons;
    std::unordered_set<int> previousButtons;

    glm::vec2 mousePos = { 0.0f, 0.0f };
    glm::vec2 lastMousePos = { 0.0f, 0.0f };
    glm::vec2 mouseDelta = { 0.0f, 0.0f };
    bool firstFrame = true;
    bool cursorCaptured = false;

    // Keys and buttons we bother tracking
    static constexpr int TRACKED_KEYS[] = {
        GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
        GLFW_KEY_Q, GLFW_KEY_E,
        GLFW_KEY_ESCAPE, GLFW_KEY_F1,
        GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE
    };
    static constexpr int TRACKED_BUTTONS[] = {
        GLFW_MOUSE_BUTTON_LEFT,
        GLFW_MOUSE_BUTTON_RIGHT,
        GLFW_MOUSE_BUTTON_MIDDLE
    };
};
