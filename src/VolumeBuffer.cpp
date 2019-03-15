//
// class to represent a 3D volume (fbo and associated textures)
//
// Author: Simon Green
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
//#include <Cg/cgGL.h>

#include "VolumeBuffer.h"

#include <glm/glm.hpp>

VolumeBuffer::VolumeBuffer(GLint format, int width, int height, int depth, int banks)
    : m_width(width),
      m_height(height),
      m_depth(depth),
      m_banks(banks),
      m_blendMode(BLEND_ADDITIVE)
{
    // create fbo
    m_fbo = new FramebufferObject();

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_max_banks);
    if (m_banks > m_max_banks) m_banks = m_max_banks;

    // create textures
    m_tex = new GLuint [m_banks];
    for(int i=0; i<m_banks; i++) {
        m_tex[i] = create3dTexture(format, m_width, m_height, m_depth);
    }

    // attach slice 0 of first texture to fbo for starters
    m_fbo->Bind();
    m_fbo->AttachTexture(GL_TEXTURE_3D, m_tex[0], GL_COLOR_ATTACHMENT0, 0, 0);
    m_fbo->IsValid();
    m_fbo->Disable();

    create2DBackground();
}

VolumeBuffer::~VolumeBuffer()
{
    delete m_fbo;
    for(int i=0; i<m_banks; i++) {
        glDeleteTextures(1, &m_tex[i]);
    }
    delete [] m_tex;
}


void VolumeBuffer::create2DBackground()
{
/////////////////////////////////////////
//
// Background SquareTexture
//
/////////////////////////////////////////



    glGenTextures(1, &m_texBkgrndID);					// Generate OpenGL texture IDs
    glBindTexture(GL_TEXTURE_2D, m_texBkgrndID);			// Bind Our Texture

    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glm::vec3 *buffer, *ptrBuffer;

    int w, h;
    w = h = 128;
    ptrBuffer = buffer = new glm::vec3[w*h];

    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++) {
             const glm::vec3 val( ((x>>2)&1) ^ ((y>>2)&1) ? glm::vec3(.7f, .7f, .7f) : glm::vec3(.2f, .2f, .2f) );
             *ptrBuffer++ = val;
        }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, buffer);

    
    delete buffer;
}

GLuint VolumeBuffer::create3dTexture(GLint internalformat, int w, int h, int d)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint mode = GL_CLAMP;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, mode);
    glTexImage3D(GL_TEXTURE_3D, 0, internalformat, w, h, d, 0, GL_LUMINANCE, GL_FLOAT, 0);
    return tex;
}

void
VolumeBuffer::setWrapMode(GLint mode, int bank)
{
    glBindTexture(GL_TEXTURE_3D, m_tex[bank]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, mode);
}

void
VolumeBuffer::setFiltering(GLint mode, int bank)
{
    glBindTexture(GL_TEXTURE_3D, m_tex[bank]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mode);
}

void
VolumeBuffer::setData(float *data, int bank)
{
    glBindTexture(GL_TEXTURE_3D, m_tex[bank]);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_width, m_height, m_depth, GL_LUMINANCE, GL_FLOAT, data);
}

// draw a slice of the volume
void
VolumeBuffer::drawSlice(float z)
{
    glBegin(GL_QUADS);
        glTexCoord3f(0.0f, 0.0f, z); glVertex2f(-1.0f, -1.0f);
        glTexCoord3f(1.0f, 0.0f, z); glVertex2f(1.0f, -1.0f);
        glTexCoord3f(1.0f, 1.0f, z); glVertex2f(1.0f, 1.0f);
        glTexCoord3f(0.0f, 1.0f, z); glVertex2f(-1.0f, 1.0f);
    glEnd();
}


#define frand() (rand() / (float) RAND_MAX) 
// run program on all slices of volume
void
VolumeBuffer::Render(ProgramObject &fprog, GLint tex1, GLint loc1, GLint tex2, GLint loc2)
{
    bind();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
/*
    if(tex1 == -2) {
        glTranslatef(.5, .5, -1.);
        //fprog.setUniform3f("centre", .4+frand()*.2, .4+frand()*.2, .4+frand()*.2);
        //fprog.setUniform1f("radius", 0.02f);
        //fprog.setUniform4f("color", frand(), frand(), frand(), 1.0f);
    }
*/
    //glScalef(.2, .2, .2);
    //glRotatef(tAngle, 0.f, 1.f, 0.f);
    //glTranslatef(0.0, 0.0, -5.0);


    glViewport(0, 0, m_width, m_height);
    glDisable(GL_DEPTH_TEST);

    switch(m_blendMode) {
    case BLEND_ADDITIVE:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }

    fprog.useProgram();


    if(tex1 > -1) {
        glUniform1i(loc1, tex1);

        glActiveTexture(GL_TEXTURE0 + tex1);
        glBindTexture(GL_TEXTURE_3D, tex1);       
    }

    if(tex2 > -1) {
        glUniform1i(loc2, tex2);

        glActiveTexture(GL_TEXTURE0 + tex2);
        glBindTexture(GL_TEXTURE_3D, tex2);       
    }



    for(int z=0; z<m_depth; z++) {
        // attach texture slice to FBO
        m_fbo->AttachTexture(GL_TEXTURE_3D, m_tex[0], GL_COLOR_ATTACHMENT0, 0, z);
        // render
        drawSlice((z + 0.5f) / (float) m_depth);
    }

    fprog.reset();


    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    unbind();
}