#pragma once
#include "UIPanel.h"
#include "../../Core/Renderer.h"
#include "../../Core/Clock.h"
#include "../../Scene/Scene.h"
#include <glm/vec3.hpp>
#include <array>
#include <string>

class CameraPanel : public UIPanel {
public:
    CameraPanel(const Renderer& renderer);

    void draw() override;
    const char* getName() const override { return "Camera"; }

private:
    const Renderer& renderer;
    glm::vec3 teleportTarget{0.0f, 0.0f, 0.0f};
};