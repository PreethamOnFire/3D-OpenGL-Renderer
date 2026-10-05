#include "ScenePanel.h"
#include "../FileDialog.h"
#include <imgui.h>
#include <filesystem>

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

ScenePanel::ScenePanel(Scene& scene, UIContext& ctx, ShaderPipeline& pipeline, MaterialLibrary& materials, Window& window)
    : scene(scene), ctx(ctx), pipeline(pipeline), materials(materials), window(window) {}

std::string ScenePanel::uniqueModelName(const std::string& base) const {
    if (!scene.getModel(base)) return base;
    for (int suffix = 2; ; ++suffix) {
        std::string candidate = base + " (" + std::to_string(suffix) + ")";
        if (!scene.getModel(candidate)) return candidate;
    }
}

void ScenePanel::drawToolbar() {
    if (ImGui::Button("Add Cube")) {
        Model* model = scene.addCube(uniqueModelName("Cube"), pipeline, materials);
        lastError = model ? "" : "Failed to add cube";
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Sphere")) {
        Model* model = scene.addSphere(uniqueModelName("Sphere"), pipeline, materials);
        lastError = model ? "" : "Failed to add sphere";
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Plane")) {
        Model* model = scene.addPlane(uniqueModelName("Plane"), pipeline, materials);
        lastError = model ? "" : "Failed to add plane";
    }
    ImGui::SameLine();
    if (ImGui::Button("Import Model...")) {
        std::string path = FileDialog::openModelFile(window.getHandle());
        if (!path.empty()) {
            std::string name = uniqueModelName(std::filesystem::path(path).stem().string());
            Model* model = scene.addModel(name, path, pipeline, materials);
            lastError = model ? "" : ("Failed to import " + path);
        }
    }

    if (!lastError.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", lastError.c_str());
    }
}

void ScenePanel::draw() {
    if (!visible) return;
    if (!ImGui::Begin(getName(), &visible)) {
        ImGui::End();
        return;
    }

    drawToolbar();
    ImGui::Separator();

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
