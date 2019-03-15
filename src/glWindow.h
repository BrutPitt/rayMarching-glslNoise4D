///////////////////////////////////////////////////////////////////////////////
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
//  
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "glslProgramObject.h"
#include "glApp.h"
#include "virtualGizmo.h"
#include "FramebufferObject.h"
#include "mNoiseClass.h"
#include "VolumeBuffer.h"
#include "VolumeRender.h"


/*
///////////////////////////////////////////
//Data Init for 32/64 bit systems
//////////////////////////////////////////

template<int> void IntDataHelper();

template<> void IntDataHelper<4>() 
{
  // do 32-bits operations
}

template<> void IntDataHelper<8>() 
{
  // do 64-bits operations
}

// helper function just to hide clumsy syntax
inline void IntData() { IntDataHelper<sizeof(size_t)>(); }
*/

#include <cstdint>
#if INTPTR_MAX == INT32_MAX 
    //APP compiling 32bit    
#elif INTPTR_MAX == INT64_MAX
    //APP compiling 32bit
#else
    #error "Environment not 32 or 64-bit."
#endif

using namespace std;

class glApp;

typedef float tbT;

class glWindow /*: public glApp*/
{

public:		

    glWindow();
	virtual ~glWindow();

    // Called when Window is created
	virtual void onInit();
    // Called when Window is closed.
	virtual void onExit();

	// The onIdle and onDisplay methods should also be overloaded.
	// Within the onIdle method put the logic of the application.
	// The onDisplay method is for any drawing code.
	virtual void onIdle();
	virtual void onRender();
	virtual void onReshape(GLint w, GLint h);

	virtual void onMouseButton(int button, int upOrDown, int x, int y);
	virtual void onMouseWheel(int wheel, int direction, int x, int y);
	virtual void onMotion(int x, int y);
	virtual void onPassiveMotion(int x, int y);

	// The onKeyDown method handles keyboard input that are standard ASCII keys
	virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
	virtual void onSpecialKeyUp(int key, int x, int y);
	virtual void onSpecialKeyDown(int key, int x, int y);

    int GetWidth()  { return theApp->GetWidth();  }
    int GetHeight() { return theApp->GetHeight(); }

    void BuildShader();
    void DrawOnTexture();

    glm::mat4 projectionMatrix; //the projection matrix
    glm::mat4 viewMatrix;       //the view matrix
    glm::mat4 modelMatrix;      // Store the model matrix
    glm::mat4 mvpMatrix;
    glm::mat4 mvMatrix;

    void togglePause() { pause ^=1; }; 

#ifdef GLAPP_USE_VIRTUALGIZMO 
    vfGizmo3DClass &getGizmo() { return gizmo; }
    void setRotation(const glm::quat &q) { getGizmo().setRotation(q); }
    glm::quat& getRotation() { return getGizmo().getRotation(); }
private:
    vfGizmo3DClass gizmo; 

#endif


private:
    bool pause = false;

	VertexShader    *vertTexBackground;
	FragmentShader  *fragTexBackground;
    //GeometryShader  *geomglslNoise;
    ProgramObject   *progTexBackground;
	ProgramObject   *progglslNoise;

    GLuint			   iTexture0;

    VolumeBuffer *m_vB;
    VolumeRender *m_vR;
    
    mNoiseClass noiseT;

    
};

