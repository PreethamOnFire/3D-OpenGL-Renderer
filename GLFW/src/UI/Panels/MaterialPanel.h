#pragma once
#include "UIPanel.h"
#include "../UIContext.h"
#include "../../Rendering/MaterialLibrary.h"

class MaterialPanel : public UIPanel {
public:
    MaterialPanel(MaterialLibrary& materials, UIContext& ctx);

    void draw() override;
    const char* getName() const override { return "Materials"; }

private:
    MaterialLibrary& materials;
    UIContext& ctx;

    void drawMaterial(Material& mat);
};
