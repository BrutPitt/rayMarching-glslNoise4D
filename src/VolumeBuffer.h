//
// class to represent a 3D volume (fbo and associated textures)
//
// Author: Simon Green
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef _VOLUME_BUFFER_H
#define _VOLUME_BUFFER_H

#include "glslProgramObject.h"
#include "glslShaderObject.h"
#include "framebufferObject.h"

// class to represent a 3D volume (fbo and associated textures)
class VolumeBuffer {
public:
    VolumeBuffer(GLint format, int width, int height, int depth, int banks);
    ~VolumeBuffer();

    enum BlendMode { BLEND_NONE = 0, BLEND_ADDITIVE };

    void bind() { m_fbo->Bind(); }
    void unbind() { m_fbo->Disable(); }

    void setModelView();
    void restoreModelView();

    void setData(float *data, int bank=0);

    void setWrapMode(GLint mode, int bank=0);
    void setFiltering(GLint mode, int bank=0);
    void setBlendMode(BlendMode mode) { m_blendMode = mode; }

    void Render(ProgramObject &fprog, GLint tex1=-1, GLint loc1=-1, GLint tex2=-1, GLint loc2=-1);

    FramebufferObject *getFBO() { return m_fbo; }
    GLuint getTexture(int bank = 0) { return m_tex[bank]; }
    GLuint getBackground() { return m_texBkgrndID; }

    int getWidth() { return m_width; }
    int getHeight() { return m_height; }
    int getDepth() { return m_depth; }

    void drawSlice(float z);

private:
    GLuint create3dTexture(GLint internalformat, int w, int h, int d);
    void create2DBackground();

    int m_width, m_height, m_depth;
    int m_max_banks;
    int m_banks;
    BlendMode m_blendMode;

    FramebufferObject *m_fbo;
    GLuint *m_tex;
    GLuint m_texBkgrndID;
};

#endif
