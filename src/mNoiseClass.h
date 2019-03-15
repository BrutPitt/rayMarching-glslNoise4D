////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#include "appDefines.h"
#include "glslProgramObject.h"
#include "glslShaderObject.h"

enum NOISE_DIM {
    NOISE_DIM1,
    NOISE_DIM2,
    NOISE_DIM3,
    NOISE_DIM4
};

class mNoiseClass
{
public:
    mNoiseClass() { frag = NULL; }
    ~mNoiseClass() { delete frag; }


    void genNoiseTex3D() 
    { 
        genNoiseTex(); 
        genGradientTex(NOISE_DIM3); 
        frag = new FragmentShader; 
        frag->Load("#define NOISE_DIM3\n", 1, SHADERS_PATH "mNoise.glsl"); 
        m_noiseType = NOISE_DIM3;
    }
    
    void genNoiseTex4D() 
    { 
        genNoiseTex(); 
        genGradientTex(NOISE_DIM4); 
        frag = new FragmentShader; 
        frag->Load("#define NOISE_DIM4\n", 1, SHADERS_PATH "mNoise.glsl");         
        m_noiseType = NOISE_DIM4;
    }


    FragmentShader* getFragment() { return frag; }


    GLuint getPremGradTex()      { return m_PremGradTex; }
    GLuint getGradientTex()      { return m_GradTex;     }
    GLuint getPernutationTex2D() { return m_PermTex2D;   }
    GLuint getPernutationTex()   { return m_PermTex;     }

    void setSamplers(ProgramObject *prg) 
    {
        prg->setUniform1i(prg->getUniformLocation("permSampler2d"  ), getPernutationTex2D());
        prg->setUniform1i(prg->getUniformLocation("permGradSampler"), getPremGradTex());
        prg->setUniform1i(prg->getUniformLocation("permSampler"    ), getPernutationTex());
        prg->setUniform1i(prg->getUniformLocation("gradSampler"    ), getGradientTex());    

        if(m_noiseType == NOISE_DIM4) {
            m_timeLoc = prg->getUniformLocation("time");
        }

    }

    GLint getTimeLoc() { return m_timeLoc; }


    void bindTextures() 
    {
        glActiveTexture(GL_TEXTURE0+ getPernutationTex2D());
        glBindTexture(GL_TEXTURE_2D, getPernutationTex2D());
        glActiveTexture(GL_TEXTURE0+ getPremGradTex());
        glBindTexture(GL_TEXTURE_2D, getPremGradTex());
        glActiveTexture(GL_TEXTURE0+ getPernutationTex());
        glBindTexture(GL_TEXTURE_2D, getPernutationTex());
        glActiveTexture(GL_TEXTURE0+ getGradientTex());
        glBindTexture(GL_TEXTURE_2D, getGradientTex());
    }


private:
    void genNoiseTex();

    void genPermutationTex();
    void genPermutationTex2D();
    void genGradientTex(NOISE_DIM size);
    void genPermGradTex();


    void GeneratePermGradTexture(float *ptrSrc, float *ptrDst) {
        *ptrDst++ = *ptrSrc++;
        *ptrDst++ = *ptrSrc++;
        *ptrDst++ = *ptrSrc++;
        *ptrDst   = *ptrSrc  ;
    }


    GLint m_timeLoc;
    GLuint m_PremGradTex;
    GLuint m_PermTex2D;
    GLuint m_GradTex;
    GLuint m_PermTex;

    NOISE_DIM m_noiseType;

    FragmentShader *frag;

};