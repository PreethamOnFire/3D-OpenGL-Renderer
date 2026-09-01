#include "RenderQueue.h"
#include <algorithm>
#include <glm/geometric.hpp>
#include "../Core/Renderer.h"
#include "../Core/ShaderPipeline.h"

void RenderQueue::add(Mesh* mesh, Material* material, const glm::vec3& camEye, const Frustum& frustum) {
    if (!mesh || !material || !material->getPipeline()) return;

    AABB worldBounds = mesh->getWorldBounds();
    if (!frustum.intersects(worldBounds.min, worldBounds.max)) return;

    float distance = glm::length(camEye - worldBounds.getCenter());
    RenderCommand command{ mesh, material, distance };

    if (material->isTransparent()) {
        transparent.push_back(command);
    } else {
        opaque.push_back(command);
    }
}

void RenderQueue::sortOpaque() {
    std::sort(opaque.begin(), opaque.end(), [](const RenderCommand& a, const RenderCommand& b) {
        unsigned int pipelineA = a.material->getPipeline()->getID();
        unsigned int pipelineB = b.material->getPipeline()->getID();
        if (pipelineA != pipelineB) return pipelineA < pipelineB;
        return a.material < b.material;
    });
}

void RenderQueue::sortTransparent() {
    std::sort(transparent.begin(), transparent.end(), [](const RenderCommand& a, const RenderCommand& b) {
        return a.distanceToCamera > b.distanceToCamera; // farthest first
    });
}

void RenderQueue::execute(Renderer& renderer) {
    Material* currentMaterial = nullptr;

    auto drawAll = [&](const std::vector<RenderCommand>& commands) {
        for (const auto& command : commands) {
            if (!command.mesh || !command.material) continue;
            if (command.material != currentMaterial) {
                if (currentMaterial) currentMaterial->unbind();
                command.material->bind();
                currentMaterial = command.material;
            }
            renderer.drawTriangles(*command.mesh, *command.material->getPipeline());
        }
    };

    drawAll(opaque);
    drawAll(transparent);

    if (currentMaterial) currentMaterial->unbind();
}
