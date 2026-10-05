#include "CubemapArray.h"
#include <GL/glew.h>

CubemapArray::CubemapArray(unsigned int size, unsigned int lightCapacity) : size(size), lightCapacity(lightCapacity) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texture);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32F, size, size,
        static_cast<GLsizei>(lightCapacity) * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // No GL_TEXTURE_COMPARE_MODE: point-light shadows here store linear distance in
    // gl_FragDepth and are sampled as a plain samplerCubeArray, compared manually.
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_NONE); // depth-only target
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CubemapArray::~CubemapArray() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (texture) glDeleteTextures(1, &texture);
}

void CubemapArray::bindFaceForWriting(unsigned int layerFace) const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0, static_cast<GLint>(layerFace));
    glViewport(0, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(size));
}

void CubemapArray::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
