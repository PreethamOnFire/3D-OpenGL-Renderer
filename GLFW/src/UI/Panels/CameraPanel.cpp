#include "CameraPanel.h"
#include <imgui.h>

CameraPanel::CameraPanel(const Renderer& renderer) : renderer(renderer) {}

void CameraPanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    Camera& camera = renderer.getCamera();

    glm::vec3 eye = camera.getEye();
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", eye.x, eye.y, eye.z);
    ImGui::Text("Yaw: %.1f  Pitch: %.1f", camera.getYaw(), camera.getPitch());
    ImGui::Separator();

    float baseSpeed = camera.getBaseSpeed();
    if (ImGui::SliderFloat("Move Speed", &baseSpeed, 0.5f, 50.0f)) {
        camera.setBaseSpeed(baseSpeed);
    }

    float sensitivity = camera.getSensitivity();
    if (ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.01f, 1.0f)) {
        camera.setSensitivity(sensitivity);
    }

    float fov = camera.getFov();
    if (ImGui::SliderFloat("FOV (deg)", &fov, 10.0f, 120.0f)) {
        camera.setFov(fov);
    }
    ImGui::Separator();

    ImGui::InputFloat3("Teleport Target", &teleportTarget.x);
    if (ImGui::Button("Teleport")) {
        camera.setEye(teleportTarget);
    }

    ImGui::End();
}
