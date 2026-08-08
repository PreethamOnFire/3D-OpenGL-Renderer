#pragma once
#include "UIContext.h"
#include "../Core/Renderer.h"
#include "../Core/Clock.h"
#include "../Core/Window.h"
#include "../Scene/Scene.h"
#include "../Rendering/MaterialLibrary.h"
#include "Panels/UIPanel.h"
#include <string>

class UIManager {
public:
    // Called once during Application::onInit()
    void init(const Window& window, const std::string& glslVersion = "#version 330");

    // Called once during Application::onShutdown()
    void shutdown();

    // Called every frame inside Application::onImGui()
    void draw(Renderer& renderer, Scene& scene,
        MaterialLibrary& materials, const Clock& clock);

    UIContext ctx;  // public so Application can read selectedModel if needed
	void registerPanel(std::unique_ptr<UIPanel> panel);

	// True while ImGui wants mouse input (hovering/clicking a panel) — callers
	// should not treat such clicks as clicks on the main viewport.
	bool wantsMouseCapture() const;

private:
	std::unordered_map<std::string, std::unique_ptr<UIPanel>> panels;
    void beginFrame();
    void endFrame();
    void drawMenuBar();  // File / View / Help menu bar at top
};