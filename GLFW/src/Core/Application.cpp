#include "Application.h"

Application::Application(int width, int height, const std::string& title) {
    window = std::make_unique<Window>(width, height, title);
    renderer = std::make_unique<Renderer>(*window->getHandle());
    input = std::make_unique<InputManager>();
    window->setResizeCallback([this](int w, int h) {
        renderer->getCamera().updateProjectionMatrix(
            static_cast<float>(w), static_cast<float>(h));
        });
    UI = std::make_unique<UIManager>();
}

void Application::run() {
	input->captureCursor(window->getHandle());
	UI->init(*window, "#version 460");
	onInit();

    while (!window->shouldClose()) {
        window->pollEvents();
        clock.tick();
        float dt = clock.getDeltaTime();

        input->update(window->getHandle());

        if (input->isCursorCaptured()) {
            if (input->isKeyJustPressed(GLFW_KEY_ESCAPE)) {
                input->releaseCursor(window->getHandle());
            }
        } else if (input->isMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && !UI->wantsMouseCapture()) {
            input->captureCursor(window->getHandle());
        }

		renderer->getCamera().update(*input, dt);

        onUpdate();

        renderer->clear();
        onRender();
        onImGui();

        window->swapBuffers();
    }

    onShutdown();
    UI->shutdown();
}
