#pragma once
#pragma once
#include <vector>
#include <memory>
#include "Core/Application.h"
#include "Scene/Scene.h"
#include "Core/ShaderPipelineLibrary.h"
#include "Rendering/MaterialLibrary.h"
#include "Rendering/RenderPass.h"
#include "Rendering/DebugPass.h"

class RendererApp : public Application {
public:
    RendererApp() : Application(1920, 1080, "Renderer") {}

protected:
    void onInit()   override;
    void onUpdate() override;
    void onRender() override;
    void onImGui()  override;   // fill this in during Phase 2

private:
    std::unique_ptr<Scene>  scene;
    ShaderPipelineLibrary pipelines;
    MaterialLibrary materials;
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    DebugSettings debugSettings;
};