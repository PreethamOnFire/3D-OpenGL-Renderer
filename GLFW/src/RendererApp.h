#pragma once
#pragma once
#include "Core/Application.h"
#include "Scene/Scene.h"
#include "Core/ShaderPipelineLibrary.h"
#include "Rendering/MaterialLibrary.h"

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
};