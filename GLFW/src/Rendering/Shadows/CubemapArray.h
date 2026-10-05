#pragma once

// Owns a single GL_TEXTURE_CUBE_MAP_ARRAY depth texture (6 layer-faces per
// shadow-casting point light) plus the one FBO used to render into whichever
// layer-face is currently targeted. Kept separate from ShadowMapArray (2D array,
// hardware shadow-compare sampling) rather than sharing a base/template with it --
// this array uses a different GL target, no compare mode (point-light shadows here
// use the linear-distance technique and a plain samplerCubeArray, sampled by
// direction rather than by lightSpaceMatrix-projected UV), and edge-clamp instead
// of border-clamp, since a direction vector always lands on some face.
class CubemapArray {
public:
    CubemapArray(unsigned int size, unsigned int lightCapacity);
    ~CubemapArray();
    CubemapArray(const CubemapArray&) = delete;
    CubemapArray& operator=(const CubemapArray&) = delete;

    // Retargets the shared FBO's depth attachment to one layer-face
    // (layerFace = lightSlot * 6 + faceIndex, faceIndex in GL's +X,-X,+Y,-Y,+Z,-Z order)
    // and binds it for writing.
    void bindFaceForWriting(unsigned int layerFace) const;
    void unbind() const;

    unsigned int getTexture() const { return texture; }
    unsigned int getSize() const { return size; }
    unsigned int getLightCapacity() const { return lightCapacity; }

private:
    unsigned int texture = 0;
    unsigned int fbo = 0;
    unsigned int size;
    unsigned int lightCapacity;
};
