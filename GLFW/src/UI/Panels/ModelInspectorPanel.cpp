#include "ModelInspectorPanel.h"
#include <imgui.h>
#include <cstring>

ModelInspectorPanel::ModelInspectorPanel(UIContext& ctx) : ctx(ctx) {}

void ModelInspectorPanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    Model* model = ctx.selectedModel;
    if (!model) {
        ImGui::TextDisabled("No model selected");
        ImGui::End();
        return;
    }

    if (model != lastModel) {
        lastModel = model;
        strncpy_s(nameBuffer.data(), nameBuffer.size(), model->getName().c_str(), _TRUNCATE);
    }

    if (ImGui::InputText("Name", nameBuffer.data(), nameBuffer.size())) {
        model->setName(nameBuffer.data());
    }

    glm::vec3 position = model->getPosition();
    if (ImGui::DragFloat3("Position", &position.x, 0.05f)) {
        model->setPosition(position);
    }

    glm::vec3 rotation = model->getRotation();
    if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f)) {
        model->setRotation(rotation);
    }

    glm::vec3 scale = model->getScale();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.05f)) {
        model->setScale(scale);
    }

    if (ImGui::Button("Reset Transform")) {
        model->setPosition(glm::vec3(0.0f));
        model->setRotation(glm::vec3(0.0f));
        model->setScale(glm::vec3(1.0f));
    }

    ImGui::SameLine();

    bool modelVisible = model->isVisible();
    if (ImGui::Checkbox("Visible", &modelVisible)) {
        model->setVisible(modelVisible);
    }

    ImGui::End();
}
