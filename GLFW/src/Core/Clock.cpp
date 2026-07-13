#include "Clock.h"
#include <GLFW/glfw3.h>

void Clock::tick() {
    currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;
}
