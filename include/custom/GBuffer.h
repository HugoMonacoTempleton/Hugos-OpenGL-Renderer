#ifndef GBUFFER_H
#define GBUFFER_H

#include <custom/RenderTexture.h>

#include <glad/glad.h>

#include <iostream>

/*-----------------------------------------------------------------------------------------------*/
/* GBUFFER: Contains the current frame buffer as well as a normal and depth buffer of the scene. */
/*-----------------------------------------------------------------------------------------------*/
class GBuffer {
public:
    RenderTexture screenBuffer;
    unsigned int normalDepthBuffer;
    unsigned int attachments[2];

    GBuffer(int width, int height) : screenBuffer { RenderTexture(width, height) } {
        
        glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.framebuffer);

        // Depth Normal buffer
        glGenTextures(1, &normalDepthBuffer);
        glBindTexture(GL_TEXTURE_2D, normalDepthBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalDepthBuffer, 0);

        attachments[0] = GL_COLOR_ATTACHMENT0;
        attachments[1] = GL_COLOR_ATTACHMENT1;
        glDrawBuffers(2, attachments);

        // Check for framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void deleteGBuffer() {
        screenBuffer.deleteRenderTex();
        glDeleteTextures(1, &normalDepthBuffer);
    }
};

#endif
