#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/Shader.h"
#include "Core/Renderer.h"
#include "Core/Window.h"
#include "Core/Clock.h"
#include "Scene/Model.h"
#include "Scene/Scene.h"
#include "Core/Application.h"
#include "Loaders/TextureLoader.h"
#include "RendererApp.h"
#include <vector>


void RendererApp::onInit() {
    scene = std::make_unique<Scene>();
    skyboxShader = std::make_unique<Shader>("src/Shaders/SkyBoxVertexShader.vs", "src/Shaders/SkyBoxFragmentShader.fs");
    objectShader = std::make_unique<Shader>("src/Shaders/DefaultVertexShader.vs", "src/Shaders/TextureShader.fs");

    std::vector<std::string> faces{
        "right.jpg", "left.jpg", "top.jpg",
        "bottom.jpg", "front.jpg", "back.jpg"
    };
    scene->setSkybox(faces, "assets/skybox", *skyboxShader);

    Model* monkey = scene->addModel("Monkey", "assets/models/Monkey.obj", *objectShader);
    monkey->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    monkey->setScale(glm::vec3(0.8f, 0.8f, 0.8f));

    Model* normandy = scene->addModel("Normandy", "assets/models/Normandy/Normandy.obj", *objectShader);
    normandy->setPosition(glm::vec3(0.0f, 13.0f, -6.0f));
    normandy->setScale(glm::vec3(0.001f, 0.001f, 0.001f));

    Model* tower = scene->addModel("Tower", "assets/models/Tower/scene.gltf", *objectShader);
    tower->setPosition(glm::vec3(-6.0f, 4.0f, 6.0f));
    tower->setScale(glm::vec3(0.001f, 0.001f, 0.001f));

    Model* island = scene->addModel("Island", "assets/models/Island/Island.obj", *objectShader);
    island->setScale(glm::vec3(10.0f, 10.0f, 10.0f));

    scene->addDirectionalLight(
        glm::vec3(0.3f, 1.0f, 0.5f),
        glm::vec3(1.0f, 0.95f, 0.8f),
        1.0f
    );
}

void RendererApp::onUpdate() {
    Model* normandy = scene->getModel("Normandy");
    if (normandy) {
        normandy->translate(glm::vec3(0.0f, 0.01f * sin(clock.getTime()), 0.0f));
    }
}


void RendererApp::onRender() {
    scene->render(*renderer);
}

void RendererApp::onImGui() {
    // Phase 2: ImGui::Begin("Scene"), list models, sliders, etc.
}


int main() {
    RendererApp app;
    app.run();
    return 0;
}
