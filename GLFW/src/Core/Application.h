#pragma once
#pragma once
#include "Window.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Clock.h"
#include <memory>
#include "../UI/UIManager.h"

class Application {
public:
    Application(int width, int height, const std::string& title);
    virtual ~Application() = default;

    void run();  // the main loop — final, not overridable

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

protected:
    // Subclass hooks — override these, not run()
    virtual void onInit() {}           // load shaders, models, set up scene
    virtual void onUpdate() {}   // game logic, camera update, input
    virtual void onRender() {}           // scene->render()
    virtual void onImGui() {}           // ImGui windows (empty until Phase 2)
    virtual void onShutdown() {}         // cleanup if needed

    // Accessible to subclass
    std::unique_ptr<Window>       window;
    std::unique_ptr<Renderer>     renderer;
    std::unique_ptr<InputManager> input;
    std::unique_ptr<UIManager> UI;
    Clock clock;
};