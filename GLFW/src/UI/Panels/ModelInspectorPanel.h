#pragma once
#include "UIPanel.h"
#include "../UIContext.h"
#include "../../Scene/Model.h"
#include <array>

class ModelInspectorPanel : public UIPanel {
public:
    ModelInspectorPanel(UIContext& ctx);

    void draw() override;
    const char* getName() const override { return "Model Inspector"; }

private:
    UIContext& ctx;
    Model* lastModel = nullptr;
    std::array<char, 128> nameBuffer{};
};
