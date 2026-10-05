#pragma once
#include "RenderPass.h"

class ShaderPipeline;
class ShadowMapArray;
class CubemapArray;

class ForwardPass : public RenderPass {
public:
    ForwardPass(ShaderPipeline& lightingPipeline, const ShadowMapArray& shadowMapArray, const CubemapArray& pointShadowMapArray);

    void execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) override;
    const char* getName() const override { return "ForwardPass"; }

private:
    ShaderPipeline* lightingPipeline;
    const ShadowMapArray* shadowMapArray;
    const CubemapArray* pointShadowMapArray;
};
