#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void setTitle(const std::string& title); // useful for FPS counter later

    GLFWwindow* getHandle() const { return handle; }
    int getWidth()  const { return width; }
    int getHeight() const { return height; }

    void onResize(int newWidth, int newHeight);


    void setResizeCallback(std::function<void(int, int)> cb);

    void setVSync(bool enabled);
    bool isVSyncEnabled() const { return vsync; }

private:
    GLFWwindow* handle = nullptr;
    int width, height;
    bool vsync = true;
    std::function<void(int, int)> resizeCallback;
};

