#include "UIManager.h"
#include "../Core/Window.h"
#include "imgui.h" 
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void UIManager::init(const Window& window, const std::string& glslVersion) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // enables docking

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
}

void UIManager::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIManager::registerPanel(std::unique_ptr<UIPanel> panel) {
    panels[panel->getName()] = std::move(panel);
}

bool UIManager::wantsMouseCapture() const {
    return ImGui::GetIO().WantCaptureMouse;
}

void UIManager::draw(Renderer& renderer, Scene& scene,
    MaterialLibrary& materials, const Clock& clock) {
    beginFrame();
    drawMenuBar();

    // Dockspace — lets panels be dragged and docked like a real editor
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
        ImGuiDockNodeFlags_PassthruCentralNode);

	for (const auto& [name, panel] : panels) {
		if (panel->isVisible()) {
			panel->draw();
		}
	}

    endFrame();
}

void UIManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::drawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                // Handle exit logic
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
			for (const auto& [name, panel] : panels) {
				bool isVisible = panel->isVisible();
				if (ImGui::MenuItem(name.c_str(), nullptr, &isVisible)) {
					panel->setVisible(isVisible);
				}
			}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}