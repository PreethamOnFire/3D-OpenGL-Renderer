#pragma once
#include "UIPanel.h"
#include "../UIContext.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneNode.h"
#include "../../Core/ShaderPipeline.h"
#include "../../Rendering/MaterialLibrary.h"
#include "../../Core/Window.h"

class ScenePanel : public UIPanel {
public:
    ScenePanel(Scene& scene, UIContext& ctx, ShaderPipeline& pipeline, MaterialLibrary& materials, Window& window);

    void draw() override;
    const char* getName() const override { return "Scene"; }

private:
    Scene& scene;
    UIContext& ctx;
    ShaderPipeline& pipeline;
    MaterialLibrary& materials;
    Window& window;

    std::string lastError;

    void drawToolbar();
    void drawNode(SceneNode* node);
    std::string uniqueModelName(const std::string& base) const;
};
