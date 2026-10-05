#pragma once
#include <vector>
#include <glm/glm.hpp>

enum class AttachmentFormat {
    RGBA8,      // standard 8-bit color (albedo, final output)
    RGB16F,     // HDR color / world-space position, normals
    RGBA16F,
    Depth24,    // standard depth
	Depth32,    // People don't usually need this, but it's here for completeness
    Depth32F,    // higher precision depth (shadow maps benefit from this)
	Stencil8,   // stencil buffer (usually used with depth)
};

enum class FilterMode {
    Nearest,    // G-buffer style attachments — don't blend encoded/discrete data
    Linear,     // color attachments meant to be sampled smoothly (bloom, blur passes);
                // combined with compareMode on a depth attachment, gives free hardware 2x2 PCF
};

enum class WrapMode {
    ClampToEdge,    // default — smears edge texels when sampled out of bounds
    ClampToBorder,  // sampling outside returns borderColor; use for shadow maps so
                    // out-of-frustum samples read as "not in shadow" instead of smearing
    Repeat,
};

struct AttachmentSpec {
    AttachmentFormat format;
    FilterMode filter = FilterMode::Nearest;
    WrapMode wrap = WrapMode::ClampToEdge;
    glm::vec4 borderColor = glm::vec4(1.0f); // only applied when wrap == ClampToBorder
    bool compareMode = false; // depth formats only: enables sampler2DShadow comparison sampling
};

struct FrameBufferSpec {
    unsigned int width = 0;
    unsigned int height = 0;
    std::vector<AttachmentSpec> attachments; // order matters — index = attachment slot
};

class FrameBuffer
{
public:
    explicit FrameBuffer(const FrameBufferSpec& spec);
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void bind() const;
    void unbind() const;
    void resize(unsigned int width, unsigned int height);

    unsigned int getColorAttachment(size_t index = 0) const;
    unsigned int getDepthAttachment() const;
    unsigned int getWidth() const { return spec.width; }
    unsigned int getHeight() const { return spec.height; }
    bool hasDepthAttachment() const { return depthAttachment != 0; }

private:
    void invalidate(); // (re)creates the GL objects from spec
    void destroy();     // deletes current GL objects, used by both ~FrameBuffer and resize

    unsigned int fbo = 0;
    std::vector<unsigned int> colorAttachments; // textures, index-aligned with spec.attachments (color ones only)
    unsigned int depthAttachment = 0;
    FrameBufferSpec spec;

};

