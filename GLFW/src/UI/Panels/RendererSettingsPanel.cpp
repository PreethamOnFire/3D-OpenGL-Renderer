#include "RendererSettingsPanel.h"
#include <imgui.h>

RendererSettingsPanel::RendererSettingsPanel(Renderer& renderer, Window& window, DebugSettings& debugSettings)
    : renderer(renderer), window(window), debugSettings(debugSettings) {}

void RendererSettingsPanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    bool wireframe = renderer.isWireframe();
    if (ImGui::Checkbox("Wireframe", &wireframe)) {
        renderer.setWireframe(wireframe);
    }

    glm::vec3 clearColor = renderer.getClearColor();
    if (ImGui::ColorEdit3("Clear Color", &clearColor.x)) {
        renderer.setClearColor(clearColor);
    }

    bool vsync = window.isVSyncEnabled();
    if (ImGui::Checkbox("VSync", &vsync)) {
        window.setVSync(vsync);
    }

    if (ImGui::CollapsingHeader("Debug Overlay")) {
        ImGui::Checkbox("Enabled", &debugSettings.enabled);
        if (debugSettings.enabled) {
            ImGui::Checkbox("Lights", &debugSettings.showLights);
            ImGui::Checkbox("Model Bounds", &debugSettings.showModelBounds);
            ImGui::Checkbox("Mesh Bounds", &debugSettings.showMeshBounds);
            ImGui::Checkbox("Draw On Top", &debugSettings.drawOnTop);
        }
    }

    ImGui::End();
}
