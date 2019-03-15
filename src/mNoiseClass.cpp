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
#include <stdlib.h>

#include "mNoiseClass.h"

static unsigned char permutation[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// gradients for 3d noise
static float g3[] = {
    1,1,0,
    -1,1,0,
    1,-1,0,
    -1,-1,0,
    1,0,1,
    -1,0,1,
    1,0,-1,
    -1,0,-1, 
    0,1,1,
    0,-1,1,
    0,1,-1,
    0,-1,-1,
    1,1,0,
    0,-1,1,
    -1,1,0,
    0,-1,-1,
};

// gradients for 4D noise
static float g4[] = {
	0, -1, -1, -1,
	0, -1, -1, 1,
	0, -1, 1, -1,
	0, -1, 1, 1,
	0, 1, -1, -1,
	0, 1, -1, 1,
	0, 1, 1, -1,
	0, 1, 1, 1,
	-1, -1, 0, -1,
	-1, 1, 0, -1,
	1, -1, 0, -1,
	1, 1, 0, -1,
	-1, -1, 0, 1,
	-1, 1, 0, 1,
	1, -1, 0, 1,
	1, 1, 0, 1,
	
	-1, 0, -1, -1,
	1, 0, -1, -1,
	-1, 0, -1, 1,
	1, 0, -1, 1,
	-1, 0, 1, -1,
	1, 0, 1, -1,
	-1, 0, 1, 1,
	1, 0, 1, 1,
	0, -1, -1, 0,
	0, -1, -1, 0,
	0, -1, 1, 0,
	0, -1, 1, 0,
	0, 1, -1, 0,
	0, 1, -1, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,
};



void mNoiseClass::genNoiseTex()
{
    genPermutationTex();
    genPermutationTex2D();
    genPermGradTex();
}

void mNoiseClass::genPermutationTex()
{
    glGenTextures(1, &m_PermTex);
    glBindTexture(GL_TEXTURE_2D, m_PermTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, permutation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

}


void mNoiseClass::genGradientTex(NOISE_DIM size)
{

    

    glGenTextures(1, &m_GradTex);
    glBindTexture(GL_TEXTURE_2D, m_GradTex);
  
    if(size == NOISE_DIM4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 1, 0, GL_RGBA, GL_FLOAT, g4);
    else                   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB , 16, 1, 0, GL_RGB,  GL_FLOAT, g3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);



}

inline int perm(int i) { return permutation[i & 0xff]; }
    
inline void perm2D(int x, int y, unsigned char *ptr) 
{
	    int A =  perm(x) + y;
	    int AA = perm(A);
	    int AB = perm(A + 1);
  	    int B =  perm(x + 1) + y;
  	    int BA = perm(B);
  	    int BB = perm(B + 1);
        *ptr++ = AA;
        *ptr++ = AB;
        *ptr++ = BA;
        *ptr   = BB;
}



void mNoiseClass::genPermutationTex2D()
{


    unsigned char *TexData2D = new unsigned char [256*256*4];
    unsigned char *ptr = TexData2D;

    for(int y=0; y<256; y++)
        for(int x=0; x<256; x++, ptr+=4)  
            perm2D(x,y,ptr);

    glGenTextures(1, &m_PermTex2D);
    glBindTexture(GL_TEXTURE_2D, m_PermTex2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexData2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    delete TexData2D;

}


void mNoiseClass::genPermGradTex()
{

    float *PermGrad = new float [256*4];

    for(int x=0; x<256; x++) 
        memcpy(PermGrad+x*4, g4 + (permutation[x] & 0x1f) * 4, 4*sizeof(float));

    glGenTextures(1, &m_PremGradTex);
    glBindTexture(GL_TEXTURE_2D, m_PremGradTex);        

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 1, 0, GL_RGBA, GL_FLOAT, PermGrad);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    delete PermGrad;
}