#pragma once
#include "../Scene/Model.h"

struct UIContext {
    Model* selectedModel = nullptr;
    int     selectedLightIndex = -1;
    bool    showPerformance = true;
    bool    showScenePanel = true;
    bool    showInspector = true;
    bool    showLighting = true;
    bool    showMaterials = true;
};
