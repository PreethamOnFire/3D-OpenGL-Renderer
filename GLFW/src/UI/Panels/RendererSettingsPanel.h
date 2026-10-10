#pragma once
#include "UIPanel.h"
#include "../../Core/Renderer.h"
#include "../../Core/Window.h"
#include "../../Rendering/DebugPass.h"

class RendererSettingsPanel : public UIPanel {
public:
    RendererSettingsPanel(Renderer& renderer, Window& window, DebugSettings& debugSettings);

    void draw() override;
    const char* getName() const override { return "Renderer Settings"; }

private:
    Renderer& renderer;
    Window& window;
    DebugSettings& debugSettings;
};
