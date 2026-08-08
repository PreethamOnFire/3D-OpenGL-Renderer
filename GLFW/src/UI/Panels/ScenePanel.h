#pragma once
#include "UIPanel.h"
#include "../UIContext.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneNode.h"

class ScenePanel : public UIPanel {
public:
    ScenePanel(Scene& scene, UIContext& ctx);

    void draw() override;
    const char* getName() const override { return "Scene"; }

private:
    Scene& scene;
    UIContext& ctx;

    void drawNode(SceneNode* node);
};
