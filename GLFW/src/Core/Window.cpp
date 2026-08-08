#include "Window.h"
#include <iostream>
#include <stdexcept>

// Static pointer used in GLFW callbacks — callbacks can't be member functions
// so we store the instance here and forward to the member callback handlers
static Window* s_instance = nullptr;

static void framebufferResizeCallback(GLFWwindow* handle, int width, int height) {
    if (s_instance)
        s_instance->onResize(width, height);
}

static void errorCallback(int error, const char* description) {
    std::cerr << "[GLFW Error " << error << "] " << description << std::endl;
}

Window::Window(int width, int height, const std::string& title)
    : width(width), height(height) {

    s_instance = this;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(handle);
    setVSync(true);

    // GLEW needs the context current before init
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        throw std::runtime_error(
            std::string("Failed to initialize GLEW: ") +
            reinterpret_cast<const char*>(glewGetErrorString(glewStatus))
        );
    }

    // Register resize callback
    glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);

    std::cout << "[Window] OpenGL " << glGetString(GL_VERSION)
        << " | " << glGetString(GL_RENDERER) << std::endl;
}

Window::~Window() {
    if (handle) {
        glfwDestroyWindow(handle);
        handle = nullptr;
    }
    glfwTerminate();
    s_instance = nullptr;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

void Window::swapBuffers() {
    glfwSwapBuffers(handle);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::setTitle(const std::string& title) {
    glfwSetWindowTitle(handle, title.c_str());
}


void Window::onResize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, width, height);

    if (resizeCallback)
        resizeCallback(width, height);
}

void Window::setResizeCallback(std::function<void(int, int)> cb) {
    resizeCallback = cb;
}

void Window::setVSync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
    vsync = enabled;
}
