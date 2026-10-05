#pragma once

// Owns a single GL_TEXTURE_2D_ARRAY depth texture (one layer per shadow-casting
// light) plus the one FBO used to render into whichever layer is currently
// targeted. This is deliberately separate from FrameBuffer, which only deals in
// single 2D attachments -- a layered render target needs glFramebufferTextureLayer
// instead of a fixed attachment, so it gets its own minimal type rather than
// growing FrameBuffer's responsibilities to cover both cases.
class ShadowMapArray {
public:
    ShadowMapArray(unsigned int size, unsigned int layerCount);
    ~ShadowMapArray();
    ShadowMapArray(const ShadowMapArray&) = delete;
    ShadowMapArray& operator=(const ShadowMapArray&) = delete;

    // Retargets the shared FBO's depth attachment to `layer` and binds it for writing.
    void bindLayerForWriting(unsigned int layer) const;
    void unbind() const;

    unsigned int getTexture() const { return texture; }
    unsigned int getSize() const { return size; }
    unsigned int getLayerCount() const { return layerCount; }

private:
    unsigned int texture = 0;
    unsigned int fbo = 0;
    unsigned int size;
    unsigned int layerCount;
};
