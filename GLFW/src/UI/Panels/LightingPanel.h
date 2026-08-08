#pragma once
#include "UIPanel.h"
#include "../UIContext.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Light.h"

class LightingPanel : public UIPanel {
public:
    LightingPanel(Scene& scene, UIContext& ctx);

    void draw() override;
    const char* getName() const override { return "Lighting"; }

private:
    Scene& scene;
    UIContext& ctx;

    glm::vec3 ambientColor{ 1.0f };
    float ambientIntensity = 1.0f;
    bool ambientInitialized = false;

    void drawAmbientSection();
    void drawLightList();
    void drawLight(Light& light, int index, bool& removeRequested, int& removeIndex);
    void updateAmbient();

    static const char* lightTypeLabel(LightType type);
};
