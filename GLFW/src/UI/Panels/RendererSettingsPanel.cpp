#include "RendererSettingsPanel.h"
#include <imgui.h>

RendererSettingsPanel::RendererSettingsPanel(Renderer& renderer, Window& window)
    : renderer(renderer), window(window) {}

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

    ImGui::End();
}
