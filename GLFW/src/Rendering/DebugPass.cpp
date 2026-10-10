#include "DebugPass.h"
#include <GL/glew.h>
#include <cmath>
#include <algorithm>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "../Scene/Scene.h"
#include "../Core/ShaderPipeline.h"
#include "../UI/UIContext.h"

namespace {
    constexpr size_t kInitialCapacity = 1024; // vertices

    const glm::vec3 kModelBoundsColor(1.0f, 0.9f, 0.1f);
    const glm::vec3 kSelectedModelBoundsColor(1.0f, 0.5f, 0.0f);
    const glm::vec3 kMeshBoundsColor(0.0f, 0.6f, 0.6f);

    constexpr float kPointLightRadius = 0.5f;
    constexpr float kSpotConeLength = 3.0f;
    constexpr float kDirectionalAnchorDistance = 8.0f;
    constexpr float kDirectionalArrowLength = 2.0f;

    // Any two unit vectors perpendicular to n (and to each other).
    void makeBasis(const glm::vec3& n, glm::vec3& u, glm::vec3& v) {
        glm::vec3 axis = std::abs(n.y) < 0.99f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        u = glm::normalize(glm::cross(axis, n));
        v = glm::cross(n, u);
    }
}

DebugPass::DebugPass(ShaderPipeline& debugPipeline, const DebugSettings& settings, const UIContext& ctx)
    : debugPipeline(&debugPipeline), settings(settings), ctx(ctx), capacity(kInitialCapacity) {
    vao = std::make_unique<VertexArray>();
    vbo = std::make_unique<VertexBuffer>(nullptr, capacity * sizeof(DebugVertex), true);
    vao->addVertexBuffer(*vbo, { 3, 3 }); // position, color
    vao->unbind();
}

DebugPass::~DebugPass() = default;

void DebugPass::addLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color) {
    vertices.push_back({ a, color });
    vertices.push_back({ b, color });
}

void DebugPass::addBox(const AABB& box, const glm::vec3& color) {
    if (!box.isValid()) return;
    const glm::vec3& lo = box.min;
    const glm::vec3& hi = box.max;
    glm::vec3 c[8] = {
        { lo.x, lo.y, lo.z }, { hi.x, lo.y, lo.z }, { hi.x, hi.y, lo.z }, { lo.x, hi.y, lo.z },
        { lo.x, lo.y, hi.z }, { hi.x, lo.y, hi.z }, { hi.x, hi.y, hi.z }, { lo.x, hi.y, hi.z },
    };
    for (int i = 0; i < 4; ++i) {
        addLine(c[i], c[(i + 1) % 4], color);         // back face
        addLine(c[i + 4], c[(i + 1) % 4 + 4], color); // front face
        addLine(c[i], c[i + 4], color);               // connecting edges
    }
}

void DebugPass::addCross(const glm::vec3& pos, float size, const glm::vec3& color) {
    addLine(pos - glm::vec3(size, 0.0f, 0.0f), pos + glm::vec3(size, 0.0f, 0.0f), color);
    addLine(pos - glm::vec3(0.0f, size, 0.0f), pos + glm::vec3(0.0f, size, 0.0f), color);
    addLine(pos - glm::vec3(0.0f, 0.0f, size), pos + glm::vec3(0.0f, 0.0f, size), color);
}

void DebugPass::addCircle(const glm::vec3& center, const glm::vec3& normal, float radius, const glm::vec3& color, int segments) {
    glm::vec3 u, v;
    makeBasis(glm::normalize(normal), u, v);
    const float step = 2.0f * 3.14159265359f / segments;
    glm::vec3 prev = center + u * radius;
    for (int i = 1; i <= segments; ++i) {
        float a = i * step;
        glm::vec3 next = center + (u * std::cos(a) + v * std::sin(a)) * radius;
        addLine(prev, next, color);
        prev = next;
    }
}

void DebugPass::addArrow(const glm::vec3& from, const glm::vec3& dir, float length, const glm::vec3& color) {
    glm::vec3 d = glm::normalize(dir);
    glm::vec3 tip = from + d * length;
    addLine(from, tip, color);

    glm::vec3 u, v;
    makeBasis(d, u, v);
    float headLen = length * 0.2f;
    float headWidth = headLen * 0.5f;
    glm::vec3 headBase = tip - d * headLen;
    addLine(tip, headBase + u * headWidth, color);
    addLine(tip, headBase - u * headWidth, color);
    addLine(tip, headBase + v * headWidth, color);
    addLine(tip, headBase - v * headWidth, color);
}

void DebugPass::addMeshBounds(const SceneNode& node, const glm::vec3& color) {
    for (const auto& mesh : node.getMeshes()) {
        addBox(mesh->getWorldBounds(), color);
    }
    for (const auto& child : node.getChildren()) {
        addMeshBounds(*child, color);
    }
}

void DebugPass::upload() {
    vbo->bind();
    if (vertices.size() > capacity) {
        while (capacity < vertices.size()) capacity *= 2;
        glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(DebugVertex), vertices.data());
}

void DebugPass::execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) {
    if (!settings.enabled) return;
    vertices.clear();

    const Camera& camera = renderer.getCamera();
    const glm::mat4& view = camera.getViewMatrix();

    if (settings.showLights) {
        // Camera position/forward from the inverse view matrix, used to anchor directional-light
        // arrows (which have no position of their own) somewhere always in view.
        glm::mat4 invView = glm::inverse(view);
        glm::vec3 camPos = glm::vec3(invView[3]);
        glm::vec3 camForward = -glm::normalize(glm::vec3(invView[2]));
        glm::vec3 directionalAnchor = camPos + camForward * kDirectionalAnchorDistance;

        const auto& lights = scene.getLights();
        for (size_t i = 0; i < lights.size(); ++i) {
            const Light& light = lights[i];
            glm::vec3 color = light.color;

            switch (light.type) {
            case LightType::POINT:
                addCross(light.position, kPointLightRadius, color);
                addCircle(light.position, glm::vec3(0.5f, 0.0f, 0.0f), kPointLightRadius, color);
                addCircle(light.position, glm::vec3(0.0f, 0.5f, 0.0f), kPointLightRadius, color);
                addCircle(light.position, glm::vec3(0.0f, 0.0f, 0.5f), kPointLightRadius, color);
                break;

            case LightType::SPOT: {
                glm::vec3 dir = glm::normalize(light.direction);
                addCross(light.position, kPointLightRadius * 0.5f, color);
                addArrow(light.position, dir, kSpotConeLength * 0.5f, color);

                glm::vec3 coneEnd = light.position + dir * kSpotConeLength;
                // Cutoffs are stored as cosines; cap the angle so a 90-degree cone doesn't blow up tan().
                auto coneRadius = [](float cosCutoff) {
                    float angle = std::min(std::acos(glm::clamp(cosCutoff, -1.0f, 1.0f)), glm::radians(85.0f));
                    return kSpotConeLength * std::tan(angle);
                };
                float outerRadius = coneRadius(light.outerCutOff);
                float innerRadius = coneRadius(light.cutOff);
                addCircle(coneEnd, dir, outerRadius, color);
                addCircle(coneEnd, dir, innerRadius, color * 0.5f);

                glm::vec3 u, v;
                makeBasis(dir, u, v);
                addLine(light.position, coneEnd + u * outerRadius, color);
                addLine(light.position, coneEnd - u * outerRadius, color);
                addLine(light.position, coneEnd + v * outerRadius, color);
                addLine(light.position, coneEnd - v * outerRadius, color);
                break;
            }

            case LightType::DIRECTIONAL:
                // light.direction points towards the light; the arrow shows the way light travels.
                addArrow(directionalAnchor, -light.direction, kDirectionalArrowLength, color);
                break;
            }
        }
    }

    if (settings.showModelBounds || settings.showMeshBounds) {
        for (const auto& model : scene.getModels()) {
            if (!model->isVisible()) continue;
            if (settings.showMeshBounds && model->getRootNode()) {
                addMeshBounds(*model->getRootNode(), kMeshBoundsColor);
            }
            if (settings.showModelBounds) {
                glm::vec3 color = (model.get() == ctx.selectedModel) ? kSelectedModelBoundsColor : kModelBoundsColor;
                addBox(model->getWorldBounds(), color);
            }
        }
    }

    if (vertices.empty()) return;
    upload();

    debugPipeline->depthTest = !settings.drawOnTop;
    debugPipeline->bind();
    debugPipeline->setMat4("viewProj", camera.getProjectionMatrix() * view);
    vao->bind();
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
    renderer.drawCallCount++;
    vao->unbind();
    debugPipeline->restoreState();
}
