#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "Mesh.h"
#include "Material.h"
#include "Frustum.h"

class Renderer;

struct RenderCommand {
    Mesh* mesh;
    Material* material;
    float distanceToCamera; // for transparency sorting
};

class RenderQueue {
public:
    void add(Mesh* mesh, Material* material, const glm::vec3& camEye, const Frustum& frustum);
    void sortOpaque();       // by pipeline/material id — minimizes state changes
    void sortTransparent();  // back-to-front by distanceToCamera
    void execute(Renderer& renderer); // binds materials (deduped) and submits draws
    const std::vector<RenderCommand>& getOpaque() const { return opaque; }
    const std::vector<RenderCommand>& getTransparent() const { return transparent; }
    void clear() { opaque.clear(); transparent.clear(); }

private:
    std::vector<RenderCommand> opaque;
    std::vector<RenderCommand> transparent;
};
