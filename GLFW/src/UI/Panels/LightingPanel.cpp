#include "LightingPanel.h"
#include <imgui.h>

LightingPanel::LightingPanel(Scene& scene, UIContext& ctx) : scene(scene), ctx(ctx) {}

const char* LightingPanel::lightTypeLabel(LightType type) {
    switch (type) {
        case LightType::DIRECTIONAL: return "DIRECTIONAL";
        case LightType::POINT:       return "POINT";
        case LightType::SPOT:        return "SPOT";
    }
    return "UNKNOWN";
}

void LightingPanel::updateAmbient() {
    scene.setAmbientLight(ambientColor * ambientIntensity);
}

void LightingPanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    if (!ambientInitialized) {
        ambientColor = scene.getAmbientLight();
        ambientIntensity = 1.0f;
        ambientInitialized = true;
    }

    drawAmbientSection();
    ImGui::Separator();
    drawLightList();

    ImGui::End();
}

void LightingPanel::drawAmbientSection() {
    ImGui::Text("Ambient Light");
    if (ImGui::ColorEdit3("Ambient Color", &ambientColor.x)) {
        updateAmbient();
    }
    if (ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 5.0f)) {
        updateAmbient();
    }
}

void LightingPanel::drawLightList() {
    auto& lights = scene.getLights();

    ImGui::Text("Lights (%zu)", lights.size());

    if (ImGui::Button("Add Point Light")) {
        scene.addPointLight(glm::vec3(0.0f), glm::vec3(1.0f), 1.0f);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Directional Light")) {
        scene.addDirectionalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 1.0f);
    }
	ImGui::SameLine();
	if (ImGui::Button("Add Spot Light")) {
		scene.addSpotLight(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 12.5f, 17.5f, 1.0f);
	}

    bool removeRequested = false;
    int removeIndex = -1;

    ImGui::BeginChild("LightList", ImVec2(0, 350), true);
    for (size_t i = 0; i < lights.size(); i++) {
        ImGui::PushID(static_cast<int>(i));

        char label[64];
        snprintf(label, sizeof(label), "[%d] %s", static_cast<int>(i), lightTypeLabel(lights[i].type));

        bool opened = ImGui::CollapsingHeader(label);
        if (opened) {
            ctx.selectedLightIndex = static_cast<int>(i);
            ImGui::Indent();
            drawLight(lights[i], static_cast<int>(i), removeRequested, removeIndex);
            ImGui::Unindent();
        }

        ImGui::PopID();
    }
    ImGui::EndChild();

    if (removeRequested) {
        if (ctx.selectedLightIndex == removeIndex) {
            ctx.selectedLightIndex = -1;
        }
        scene.removeLight(static_cast<size_t>(removeIndex));
    }
}

void LightingPanel::drawLight(Light& light, int index, bool& removeRequested, int& removeIndex) {
    if (light.type != LightType::DIRECTIONAL) {
        ImGui::DragFloat3("Position", &light.position.x, 0.1f);
    }
    if (light.type != LightType::POINT) {
        if (ImGui::DragFloat3("Direction", &light.direction.x, 0.05f)) {
            if (glm::length(light.direction) > 0.0001f) {
                light.direction = glm::normalize(light.direction);
            }
        }
    }

    ImGui::ColorEdit3("Color", &light.color.x);
    ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);

    if (light.type == LightType::POINT || light.type == LightType::SPOT) {
        ImGui::SliderFloat("Constant", &light.constant, 0.0f, 2.0f);
        ImGui::SliderFloat("Linear", &light.linear, 0.0f, 1.0f);
        ImGui::SliderFloat("Quadratic", &light.quadratic, 0.0f, 2.0f);
    }

    if (light.type == LightType::SPOT) {
        float innerDeg = glm::degrees(glm::acos(glm::clamp(light.cutOff, -1.0f, 1.0f)));
        float outerDeg = glm::degrees(glm::acos(glm::clamp(light.outerCutOff, -1.0f, 1.0f)));

        if (ImGui::SliderFloat("Inner Cutoff (deg)", &innerDeg, 0.0f, 90.0f)) {
            light.cutOff = glm::cos(glm::radians(innerDeg));
        }
        if (ImGui::SliderFloat("Outer Cutoff (deg)", &outerDeg, 0.0f, 90.0f)) {
            light.outerCutOff = glm::cos(glm::radians(outerDeg));
        }
    }

    if (ImGui::Button("Remove")) {
        removeRequested = true;
        removeIndex = index;
    }
}
