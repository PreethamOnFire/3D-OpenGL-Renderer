#include "Application.h"

Application::Application(int width, int height, const std::string& title) {
    window = std::make_unique<Window>(width, height, title);
    renderer = std::make_unique<Renderer>(*window->getHandle());
    input = std::make_unique<InputManager>();
    window->setResizeCallback([this](int w, int h) {
        renderer->getCamera().updateProjectionMatrix(
            static_cast<float>(w), static_cast<float>(h));
        });
}

void Application::run() {
	input->captureCursor(window->getHandle());
	onInit();

    while (!window->shouldClose()) {
        window->pollEvents();
        clock.tick();
        float dt = clock.getDeltaTime();

        input->update(window->getHandle());
		renderer->getCamera().update(*input, dt);

        onUpdate();

        renderer->clear();
        onRender();
        onImGui();     // no-op until ImGui is added

        window->swapBuffers();
    }

    onShutdown();
}
