#pragma once

class Scene;
class Renderer;
class MaterialLibrary;

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual void execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) = 0;
    virtual const char* getName() const = 0;
};
