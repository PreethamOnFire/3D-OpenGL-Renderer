#pragma once
#include <glm/mat4x4.hpp>
#include "../RenderPass.h"
#include "ShadowMapArray.h"
#include "CubemapArray.h"

class ShaderPipeline;
class Light;

// Renders a depth map for every shadow-casting light into one of two shared array
// textures, reassigned fresh each frame:
//  - directional/spot lights -> ShadowMapArray (one 2D layer per light, hardware
//    shadow-compare sampling)
//  - point lights -> CubemapArray (6 layer-faces per light, linear-distance
//    technique, plain sampling) -- a point light shadows in every direction, so a
//    single 2D map can't represent it; it needs its own resource and its own
//    dedicated shadow pipeline (see pointShadowPipeline).
class ShadowPassManager : public RenderPass {
public:
    ShadowPassManager(ShaderPipeline& shadowPipeline, ShaderPipeline& pointShadowPipeline,
        unsigned int shadowMapSize = 2048, unsigned int maxShadowLights = 8,
        unsigned int pointShadowMapSize = 1024, unsigned int maxPointShadowLights = 4);

    void execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) override;
    const char* getName() const override { return "ShadowPassManager"; }

    const ShadowMapArray& getShadowMapArray() const { return shadowMapArray; }
    const CubemapArray& getPointShadowMapArray() const { return pointShadowMapArray; }

private:
    void renderDirectionalLightDepth(Light& light, unsigned int layer, Scene& scene, Renderer& renderer);
    void renderSpotLightDepth(Light& light, unsigned int layer, Scene& scene, Renderer& renderer);
    void renderPointLightDepth(Light& light, unsigned int lightSlot, Scene& scene, Renderer& renderer);

    // Frustum-culls collectShadowCasters() against lightSpaceMatrix, then renders the
    // survivors into the given array layer. Shared tail of the directional/spot methods above.
    void renderShadowCasters(const glm::mat4& lightSpaceMatrix, unsigned int layer, Scene& scene, Renderer& renderer);

    ShaderPipeline* shadowPipeline;
    ShaderPipeline* pointShadowPipeline;
    ShadowMapArray shadowMapArray;
    CubemapArray pointShadowMapArray;

    float shadowDistance = 40.0f; // how far down the camera frustum the shadow map reaches
                                   // (also used as the far plane for spot/point light projections)
    float casterPadding = 50.0f;  // extra depth toward the light, so off-screen geometry still casts
};
