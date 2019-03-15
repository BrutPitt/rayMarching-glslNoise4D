//
// class to render a 3D volume
//
// Author: Simon Green
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "VolumeBuffer.h"
#include "glslProgramObject.h"
#include "glslShaderObject.h"

// class to render a 3D volume
class VolumeRender : public ProgramObject 
{
public:
    VolumeRender(VolumeBuffer *volume);
    ~VolumeRender();

    void render();

    void setVolume(VolumeBuffer *volume) { m_volume = volume; }
    VolumeBuffer *getVolume() { return m_volume; }


    void setDensity(float x) { m_density = x; }
    void setBrightness(float x) { m_brightness = x; }

    void SolidCube ( double dSize );
    void WireCube(   double dSize );
    void SolidSphere(double radius, GLint slices, GLint stacks);
    void WireSphere( double radius, GLint slices, GLint stacks);
    void fghCircleTable(double **sint,double **cost,const int n);

    void loadPrograms();

    void toggleTransparent() { isTransparent ^= 1; }
    bool transparent() { return isTransparent; }
    void transparent(bool b) { isTransparent = b; }

private:

    VolumeBuffer *m_volume;

    VertexShader   vertShader;
    FragmentShader fragShader;

    GLuint densityLoc, brightnessLoc, locTransparent;

    float m_density, m_brightness;

    bool isTransparent = true;
};