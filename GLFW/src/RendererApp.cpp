#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/ShaderPipeline.h"
#include "Core/Renderer.h"
#include "Core/Window.h"
#include "Core/Clock.h"
#include "Scene/Model.h"
#include "Scene/Scene.h"
#include "Core/Application.h"
#include "Loaders/TextureLoader.h"
#include "RendererApp.h"
#include "UI/Panels/PerformancePanel.h"
#include "UI/Panels/CameraPanel.h"
#include "UI/Panels/RendererSettingsPanel.h"
#include "UI/Panels/ScenePanel.h"
#include "UI/Panels/ModelInspectorPanel.h"
#include "UI/Panels/LightingPanel.h"
#include "UI/Panels/MaterialPanel.h"
#include <vector>


void RendererApp::onInit() {
    scene = std::make_unique<Scene>();
    ShaderPipeline& skyboxPipeline = pipelines.load("skybox", "src/Shaders/SkyBoxVertexShader.vs", "src/Shaders/SkyBoxFragmentShader.fs");
    ShaderPipeline& objectPipeline = pipelines.load("default", "src/Shaders/DefaultVertexShader.vs", "src/Shaders/TextureShader.fs");
    materials.create("default", objectPipeline);

    std::vector<std::string> faces{
        "right.jpg", "left.jpg", "top.jpg",
        "bottom.jpg", "front.jpg", "back.jpg"
    };
    scene->setSkybox(faces, "assets/skybox", skyboxPipeline);

    Model* monkey = scene->addModel("Monkey", "assets/models/Monkey.obj", objectPipeline, materials);
    monkey->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    monkey->setScale(glm::vec3(0.8f, 0.8f, 0.8f));

    Model* normandy = scene->addModel("Normandy", "assets/models/Normandy/Normandy.obj", objectPipeline, materials);
    normandy->setPosition(glm::vec3(0.0f, 13.0f, -6.0f));
    normandy->setScale(glm::vec3(0.001f, 0.001f, 0.001f));

    Model* tower = scene->addModel("Tower", "assets/models/Tower/scene.gltf", objectPipeline, materials);
    tower->setPosition(glm::vec3(-6.0f, 4.0f, 6.0f));
    tower->setScale(glm::vec3(0.001f, 0.001f, 0.001f));

    Model* island = scene->addModel("Island", "assets/models/Island/Island.obj", objectPipeline, materials);
    island->setScale(glm::vec3(10.0f, 10.0f, 10.0f));

    scene->addDirectionalLight(
        glm::vec3(0.3f, 1.0f, 0.5f),
        glm::vec3(1.0f, 0.95f, 0.8f),
        1.0f
    );

    UI->registerPanel(std::make_unique<PerformancePanel>(*renderer, *scene, clock));
    UI->registerPanel(std::make_unique<CameraPanel>(*renderer));
    UI->registerPanel(std::make_unique<RendererSettingsPanel>(*renderer, *window));
    UI->registerPanel(std::make_unique<ScenePanel>(*scene, UI->ctx));
    UI->registerPanel(std::make_unique<ModelInspectorPanel>(UI->ctx));
    UI->registerPanel(std::make_unique<LightingPanel>(*scene, UI->ctx));
    UI->registerPanel(std::make_unique<MaterialPanel>(materials, UI->ctx));
}

void RendererApp::onUpdate() {
    Model* normandy = scene->getModel("Normandy");
    if (normandy) {
        normandy->translate(glm::vec3(0.0f, 0.01f * sin(clock.getTime()), 0.0f));
    }
}


void RendererApp::onRender() {
    scene->render(*renderer, materials, *pipelines.get("default"));
}

void RendererApp::onImGui() {
    UI->draw(*renderer, *scene, materials, clock);
}


int main() {
    RendererApp app;
    app.run();
    return 0;
}
