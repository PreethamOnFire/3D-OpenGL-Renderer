#include "MaterialPanel.h"
#include "../../Scene/Model.h"
#include <imgui.h>

MaterialPanel::MaterialPanel(MaterialLibrary& materials, UIContext& ctx) : materials(materials), ctx(ctx) {}

void MaterialPanel::draw() {
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

    std::vector<std::string> names = model->getMaterialNames();

    ImGui::BeginChild("MaterialList", ImVec2(0, 0), true);
    for (const auto& name : names) {
        Material* mat = materials.get(name);
        if (!mat) continue;

        ImGui::PushID(name.c_str());
        if (ImGui::CollapsingHeader(name.c_str())) {
            ImGui::Indent();
            drawMaterial(*mat);
            ImGui::Unindent();
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::End();
}

void MaterialPanel::drawMaterial(Material& mat) {
    glm::vec3 ambient = mat.getAmbient();
    if (ImGui::ColorEdit3("Ambient", &ambient.x)) {
        mat.setAmbient(ambient);
    }

    glm::vec3 diffuse = mat.getDiffuse();
    if (ImGui::ColorEdit3("Diffuse", &diffuse.x)) {
        mat.setDiffuse(diffuse);
    }

    glm::vec3 specular = mat.getSpecular();
    if (ImGui::ColorEdit3("Specular", &specular.x)) {
        mat.setSpecular(specular);
    }

    float shininess = mat.getShininess();
    if (ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f)) {
        mat.setShininess(shininess);
    }

    for (const Texture& tex : mat.getTextures()) {
        ImGui::Separator();
        ImGui::Text("%s: %s", tex.type.c_str(), tex.path.c_str());
        if (tex.id != 0) {
            ImGui::Image((ImTextureID)(intptr_t)tex.id, ImVec2(48, 48));
        }
    }
}
