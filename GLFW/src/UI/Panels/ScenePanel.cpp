#include "ScenePanel.h"
#include <imgui.h>

namespace {
    const char* modelTypeBadge(ModelType type) {
        switch (type) {
            case ModelType::LOADED_MODEL:     return "LOADED";
            case ModelType::PRIMITIVE_CUBE:   return "CUBE";
            case ModelType::PRIMITIVE_SPHERE: return "SPHERE";
            case ModelType::PRIMITIVE_PLANE:  return "PLANE";
            case ModelType::CUSTOM_MESH:      return "CUSTOM";
        }
        return "UNKNOWN";
    }
}

ScenePanel::ScenePanel(Scene& scene, UIContext& ctx) : scene(scene), ctx(ctx) {}

void ScenePanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    ImGui::BeginChild("ModelList", ImVec2(0, 0), true);

    bool removeRequested = false;
    unsigned int removeId = 0;

    for (const auto& modelPtr : scene.getModels()) {
        Model* model = modelPtr.get();
        if (!model) continue;

        ImGui::PushID(static_cast<int>(model->getId()));

        bool isSelected = (ctx.selectedModel == model);
        if (ImGui::Selectable(model->getName().c_str(), isSelected, ImGuiSelectableFlags_AllowOverlap)) {
            ctx.selectedModel = model;
        }

        ImGui::SameLine();
        ImGui::TextDisabled("[%s]", modelTypeBadge(model->getModelType()));

        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            removeRequested = true;
            removeId = model->getId();
        }

        if (ImGui::TreeNode("Hierarchy")) {
            if (SceneNode* root = model->getRootNode()) {
                for (const auto& child : root->getChildren()) {
                    drawNode(child.get());
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    ImGui::EndChild();

    if (removeRequested) {
        if (ctx.selectedModel && ctx.selectedModel->getId() == removeId) {
            ctx.selectedModel = nullptr;
        }
        scene.removeModel(removeId);
    }

    ImGui::End();
}

void ScenePanel::drawNode(SceneNode* node) {
    if (!node) return;

    ImGui::PushID(node);
    bool opened = ImGui::TreeNodeEx(node->getName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
    if (opened) {
        for (const auto& child : node->getChildren()) {
            drawNode(child.get());
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}
