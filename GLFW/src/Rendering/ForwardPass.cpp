#include "ForwardPass.h"
#include <GL/glew.h>
#include "Shadows/ShadowMapArray.h"
#include "Shadows/CubemapArray.h"
#include "../Scene/Scene.h"
#include "../Core/ShaderPipeline.h"

namespace {
    // Texture units 0-2 are reserved for Material's diffuse/specular/normal maps
    // (see Material::setTexture).
    constexpr int kShadowMapArrayTextureUnit = 3;
    constexpr int kPointShadowMapArrayTextureUnit = 4;
}

ForwardPass::ForwardPass(ShaderPipeline& lightingPipeline, const ShadowMapArray& shadowMapArray, const CubemapArray& pointShadowMapArray)
    : lightingPipeline(&lightingPipeline), shadowMapArray(&shadowMapArray), pointShadowMapArray(&pointShadowMapArray) {}

void ForwardPass::execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) {
    lightingPipeline->use();
    glActiveTexture(GL_TEXTURE0 + kShadowMapArrayTextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray->getTexture());
    lightingPipeline->setInt("shadowMapArray", kShadowMapArrayTextureUnit);

    glActiveTexture(GL_TEXTURE0 + kPointShadowMapArrayTextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointShadowMapArray->getTexture());
    lightingPipeline->setInt("pointShadowMapArray", kPointShadowMapArrayTextureUnit);

    scene.render(renderer, materials, *lightingPipeline);
}
