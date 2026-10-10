#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "RenderPass.h"
#include "AABB.h"

class ShaderPipeline;
class VertexArray;
class VertexBuffer;
class SceneNode;
struct UIContext;

// Owned by the app, edited from the Renderer Settings panel, read by DebugPass each frame.
struct DebugSettings {
    bool enabled = true;
    bool showLights = true;
    bool showModelBounds = false;
    bool showMeshBounds = false;   // per-mesh AABBs -- the same boxes frustum culling tests against
    bool drawOnTop = false;        // false = depth-tested against the scene
};

// Runs after ForwardPass. Every gizmo line for the frame is appended to one CPU-side vector,
// uploaded into a single dynamic VBO, and drawn with one glDrawArrays(GL_LINES).
class DebugPass : public RenderPass {
public:
    DebugPass(ShaderPipeline& debugPipeline, const DebugSettings& settings, const UIContext& ctx);
    ~DebugPass() override;

    void execute(Scene& scene, Renderer& renderer, const MaterialLibrary& materials) override;
    const char* getName() const override { return "DebugPass"; }

private:
    struct DebugVertex {
        glm::vec3 pos;
        glm::vec3 color;
    };

    void addLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color);
    void addBox(const AABB& box, const glm::vec3& color);
    void addCross(const glm::vec3& pos, float size, const glm::vec3& color);
    void addCircle(const glm::vec3& center, const glm::vec3& normal, float radius, const glm::vec3& color, int segments = 24);
    void addArrow(const glm::vec3& from, const glm::vec3& dir, float length, const glm::vec3& color);
    void addMeshBounds(const SceneNode& node, const glm::vec3& color);

    void upload();

    ShaderPipeline* debugPipeline;
    const DebugSettings& settings;
    const UIContext& ctx;

    std::vector<DebugVertex> vertices;
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<VertexBuffer> vbo;
    size_t capacity; // in vertices
};
