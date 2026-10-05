#include "FrameBuffer.h"
#include <GL/glew.h>
#include <iostream>
namespace {
    bool isDepthFormat(AttachmentFormat fmt) {
        return fmt == AttachmentFormat::Depth24 || fmt == AttachmentFormat::Depth32F;
    }

    struct GLFormatInfo { GLenum internalFormat; GLenum format; GLenum type; };

    GLenum glFilterFor(FilterMode mode) {
        return mode == FilterMode::Linear ? GL_LINEAR : GL_NEAREST;
    }

    GLenum glWrapFor(WrapMode mode) {
        switch (mode) {
        case WrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
        case WrapMode::Repeat:        return GL_REPEAT;
        case WrapMode::ClampToEdge:   default: return GL_CLAMP_TO_EDGE;
        }
    }

    GLFormatInfo glFormatFor(AttachmentFormat fmt) {
        switch (fmt) {
        case AttachmentFormat::RGBA8:   return { GL_RGBA8,   GL_RGBA, GL_UNSIGNED_BYTE };
        case AttachmentFormat::RGB16F:  return { GL_RGB16F,  GL_RGB,  GL_FLOAT };
        case AttachmentFormat::RGBA16F: return { GL_RGBA16F, GL_RGBA, GL_FLOAT };
        case AttachmentFormat::Depth24: return { GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT };
        case AttachmentFormat::Depth32F:return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
		case AttachmentFormat::Stencil8: return { GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE };
        }
        return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
    }
}

FrameBuffer::FrameBuffer(const FrameBufferSpec& spec) : spec(spec) {
    invalidate();
}
FrameBuffer::~FrameBuffer() {
    destroy();
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, spec.width, spec.height);
}
void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::resize(unsigned int width, unsigned int height) {
	if (width == spec.width && height == spec.height) return;
    if (width <= 0 || height <= 0) return;
	spec.width = width;
	spec.height = height;
	invalidate();
}
unsigned int FrameBuffer::getColorAttachment(size_t index) const {
    return index < colorAttachments.size() ? colorAttachments[index] : 0;
}
unsigned int FrameBuffer::getDepthAttachment() const {
    return depthAttachment;
}
void FrameBuffer::invalidate() {
    destroy();
    
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    std::vector<GLenum> drawBuffers;
    for (const auto& attachment : spec.attachments) {
        GLFormatInfo info = glFormatFor(attachment.format);
        unsigned int texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, info.internalFormat, spec.width, spec.height, 0,
            info.format, info.type, nullptr);

        GLenum glFilter = glFilterFor(attachment.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);

        GLenum glWrap = glWrapFor(attachment.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
        if (attachment.wrap == WrapMode::ClampToBorder) {
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &attachment.borderColor[0]);
        }

        if (isDepthFormat(attachment.format)) {
            if (attachment.compareMode) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);
            depthAttachment = texID;
        }
        else {
            GLenum slot = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(colorAttachments.size());
            glFramebufferTexture2D(GL_FRAMEBUFFER, slot, GL_TEXTURE_2D, texID, 0);
            colorAttachments.push_back(texID);
            drawBuffers.push_back(slot);
        }
    }

    if (drawBuffers.empty()) {
        // depth-only target (shadow maps) — no color output at all
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    else {
        glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FrameBuffer: incomplete framebuffer!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void FrameBuffer::destroy() {
    if (!colorAttachments.empty())
        glDeleteTextures(static_cast<GLsizei>(colorAttachments.size()), colorAttachments.data());
    if (depthAttachment)
        glDeleteTextures(1, &depthAttachment);
    if (fbo)
        glDeleteFramebuffers(1, &fbo);

    colorAttachments.clear();
    depthAttachment = 0;
    fbo = 0;
}