#pragma once
#include "UIPanel.h"
#include "../../Core/Renderer.h"
#include "../../Core/Clock.h"
#include "../../Scene/Scene.h"
#include <array>
#include <string>

class PerformancePanel : public UIPanel {
public:
    PerformancePanel(const Renderer& renderer, const Scene& scene, const Clock& clock);

    void draw() override;
    const char* getName() const override { return "Performance"; }

private:
    const Renderer& renderer;
    const Scene& scene;
    const Clock& clock;

    static constexpr int FpsHistorySize = 120;
    std::array<float, FpsHistorySize> fpsHistory{};
    int fpsHistoryOffset = 0;

    std::string gpuVendor;
    std::string gpuRenderer;
    std::string glVersion;
};
