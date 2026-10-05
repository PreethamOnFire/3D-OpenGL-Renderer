#include "ShadowPassManager.h"
#include <array>
#include <cmath>
#include <limits>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../Mesh.h"
#include "../Frustum.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Light.h"
#include "../../Core/Renderer.h"
#include "../../Core/Camera.h"
#include "../../Core/ShaderPipeline.h"

namespace {
    glm::vec3 ndcToWorld(const glm::mat4& invViewProj, const glm::vec3& ndc) {
        glm::vec4 world = invViewProj * glm::vec4(ndc, 1.0f);
        return glm::vec3(world) / world.w;
    }

    struct CubeFace {
        glm::vec3 direction;
        glm::vec3 up;
    };

    // OpenGL's defined cubemap face order: +X,-X,+Y,-Y,+Z,-Z.
    const std::array<CubeFace, 6> kCubeFaces = { {
        { glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) },
        { glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) },
        { glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f) },
        { glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f) },
        { glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f) },
        { glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f) },
    } };
}

ShadowPassManager::ShadowPassManager(ShaderPipeline& shadowPipeline, ShaderPipeline& pointShadowPipeline,
    unsigned int shadowMapSize, unsigned int maxShadowLights,
    unsigned int pointShadowMapSize, unsigned int maxPointShadowLights)
    : shadowPipeline(&shadowPipeline), pointShadowPipeline(&pointShadowPipeline),
      shadowMapArray(shadowMapSize, maxShadowLights),
      pointShadowMapArray(pointShadowMapSize, maxPointShadowLights) {
}

void ShadowPassManager::execute(Scene& scene, Renderer& renderer, const MaterialLibrary&) {
    auto& lights = scene.getLights();
    unsigned int nextLayer = 0;
    unsigned int nextPointSlot = 0;

    for (Light& light : lights) {
        if (!light.castsShadows) {
            light.shadowMapLayer = -1;
            light.shadowCubeLayer = -1;
            continue;
        }

        if (light.type == LightType::POINT) {
            if (nextPointSlot < pointShadowMapArray.getLightCapacity()) {
                renderPointLightDepth(light, nextPointSlot, scene, renderer);
                light.shadowCubeLayer = static_cast<int>(nextPointSlot);
                nextPointSlot++;
            } else {
                light.shadowCubeLayer = -1;
            }
            light.shadowMapLayer = -1;
            continue;
        }

        if (nextLayer < shadowMapArray.getLayerCount()) {
            if (light.type == LightType::DIRECTIONAL) {
                renderDirectionalLightDepth(light, nextLayer, scene, renderer);
            } else {
                renderSpotLightDepth(light, nextLayer, scene, renderer);
            }
            light.shadowMapLayer = static_cast<int>(nextLayer);
            nextLayer++;
        } else {
            light.shadowMapLayer = -1;
        }
        light.shadowCubeLayer = -1;
    }
}

void ShadowPassManager::renderDirectionalLightDepth(Light& light, unsigned int layer, Scene& scene, Renderer& renderer) {
    Camera& cam = renderer.getCamera();
    glm::vec3 dir = glm::normalize(light.direction);
    glm::vec3 up = std::abs(glm::dot(dir, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.99f
        ? glm::vec3(0.0f, 0.0f, 1.0f)
        : glm::vec3(0.0f, 1.0f, 0.0f);

    // Corners of the camera's view frustum in world space, with the far plane pulled in to
    // shadowDistance. The ortho box is fitted to exactly that wedge instead of a fixed box
    // around the camera, so no texels are spent on what the camera cannot see.
    glm::mat4 invViewProj = glm::inverse(cam.getProjectionMatrix() * cam.getViewMatrix());
    float camNear = cam.getNearPlane();
    float camFar = cam.getFarPlane();
    float farFraction = glm::clamp((shadowDistance - camNear) / (camFar - camNear), 0.0f, 1.0f);

    std::array<glm::vec3, 8> corners;
    size_t index = 0;
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            glm::vec3 nearCorner = ndcToWorld(invViewProj, glm::vec3(x, y, -1.0f));
            glm::vec3 farCorner = ndcToWorld(invViewProj, glm::vec3(x, y, 1.0f));
            corners[index++] = nearCorner;
            // frustum edges are straight lines and view depth varies linearly along them
            corners[index++] = nearCorner + (farCorner - nearCorner) * farFraction;
        }
    }

    glm::vec3 center(0.0f);
    for (const glm::vec3& corner : corners) center += corner;
    center /= static_cast<float>(corners.size());

    // light.direction points TOWARDS the light (matching the shader's dot(normal, lightDir)
    // convention), so the shadow camera looks along -dir: the way the light actually travels.
    glm::mat4 view = glm::lookAt(center, center - dir, up);

    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());
    for (const glm::vec3& corner : corners) {
        glm::vec3 lightSpace = glm::vec3(view * glm::vec4(corner, 1.0f));
        minBounds = glm::min(minBounds, lightSpace);
        maxBounds = glm::max(maxBounds, lightSpace);
    }

    // lookAt looks down -z, so the corner nearest the light is maxBounds.z and the farthest is
    // minBounds.z. Only the near plane needs padding: a directional light projects along z, so
    // geometry outside the x/y bounds can never cast into them, but geometry between the light
    // and the frustum can, and still has to be rendered into the map.
    glm::mat4 proj = glm::ortho(minBounds.x, maxBounds.x, minBounds.y, maxBounds.y,
        -maxBounds.z - casterPadding, -minBounds.z);
    light.lightSpaceMatrix = proj * view;

    renderShadowCasters(light.lightSpaceMatrix, layer, scene, renderer);
}

void ShadowPassManager::renderSpotLightDepth(Light& light, unsigned int layer, Scene& scene, Renderer& renderer) {
    glm::vec3 dir = glm::normalize(light.direction);
    glm::vec3 up = std::abs(glm::dot(dir, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.99f
        ? glm::vec3(0.0f, 0.0f, 1.0f)
        : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(light.position, light.position + dir, up);
    // outerCutOff (not cutOff) sizes the frustum: calculateSpotLight still lights the
    // penumbra between the inner and outer cone, so that band needs shadow coverage too.
    glm::mat4 proj = glm::perspective(glm::acos(light.outerCutOff) * 2.0f, 1.0f, 0.1f, shadowDistance);
    light.lightSpaceMatrix = proj * view;

    renderShadowCasters(light.lightSpaceMatrix, layer, scene, renderer);
}

void ShadowPassManager::renderShadowCasters(const glm::mat4& lightSpaceMatrix, unsigned int layer, Scene& scene, Renderer& renderer) {
    Frustum frustum = Frustum::fromViewProjection(lightSpaceMatrix);

    std::vector<Mesh*> casters;
    scene.collectShadowCasters(casters);

    shadowMapArray.bindLayerForWriting(layer);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowPipeline->bind();
    for (Mesh* mesh : casters) {
        if (!mesh) continue;
        AABB bounds = mesh->getWorldBounds();
        if (!frustum.intersects(bounds.min, bounds.max)) continue;
        mesh->render(*shadowPipeline, lightSpaceMatrix);
        renderer.drawCallCount++;
    }
    shadowPipeline->restoreState();
    shadowMapArray.unbind();
    renderer.restoreViewport();
}

void ShadowPassManager::renderPointLightDepth(Light& light, unsigned int lightSlot, Scene& scene, Renderer& renderer) {
    light.shadowFarPlane = shadowDistance;
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadowDistance);

    std::vector<Mesh*> casters;
    scene.collectShadowCasters(casters);

    pointShadowPipeline->bind();
    for (unsigned int face = 0; face < kCubeFaces.size(); ++face) {
        glm::mat4 view = glm::lookAt(light.position, light.position + kCubeFaces[face].direction, kCubeFaces[face].up);
        glm::mat4 faceMatrix = proj * view;
        Frustum frustum = Frustum::fromViewProjection(faceMatrix);

        pointShadowMapArray.bindFaceForWriting(lightSlot * 6 + face);
        glClear(GL_DEPTH_BUFFER_BIT);
        pointShadowPipeline->setVec3("lightPos", light.position);
        pointShadowPipeline->setFloat("farPlane", shadowDistance);
        for (Mesh* mesh : casters) {
            if (!mesh) continue;
            AABB bounds = mesh->getWorldBounds();
            if (!frustum.intersects(bounds.min, bounds.max)) continue;
            mesh->render(*pointShadowPipeline, faceMatrix);
            renderer.drawCallCount++;
        }
    }
    pointShadowPipeline->restoreState();
    pointShadowMapArray.unbind();
    renderer.restoreViewport();
}
