#include "PerformancePanel.h"
#include "../../Scene/SceneStats.h"
#include <GL/glew.h>
#include <imgui.h>
#include <cstdio>
#include <cfloat>

namespace {
    std::string glString(GLenum name) {
        const GLubyte* str = glGetString(name);
        return str ? reinterpret_cast<const char*>(str) : "Unknown";
    }
}

PerformancePanel::PerformancePanel(const Renderer& renderer, const Scene& scene, const Clock& clock)
    : renderer(renderer), scene(scene), clock(clock) {
    gpuVendor = glString(GL_VENDOR);
    gpuRenderer = glString(GL_RENDERER);
    glVersion = glString(GL_VERSION);
}

void PerformancePanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    float dt = clock.getDeltaTime();
    float fps = dt > 0.0f ? 1.0f / dt : 0.0f;

    fpsHistory[fpsHistoryOffset] = fps;
    fpsHistoryOffset = (fpsHistoryOffset + 1) % FpsHistorySize;

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", dt * 1000.0f);
    ImGui::Separator();

    char overlay[32];
    snprintf(overlay, sizeof(overlay), "%.1f FPS", fps);
    ImGui::PlotLines("##fps_graph", fpsHistory.data(), FpsHistorySize, fpsHistoryOffset,
        overlay, 0.0f, FLT_MAX, ImVec2(0, 80));
    ImGui::Separator();

    ImGui::Text("Draw Calls: %d", renderer.drawCallCount);
    ImGui::Separator();

    SceneStats stats = scene.getStats();
    ImGui::Text("Models: %zu", stats.modelCount);
    ImGui::Text("Meshes: %zu", stats.meshCount);
    ImGui::Text("Vertices: %zu", stats.vertexCount);
    ImGui::Text("Indices: %zu", stats.indexCount);
    ImGui::Separator();

    ImGui::Text("GPU Vendor: %s", gpuVendor.c_str());
    ImGui::Text("Renderer: %s", gpuRenderer.c_str());
    ImGui::Text("OpenGL: %s", glVersion.c_str());

    ImGui::End();
}
