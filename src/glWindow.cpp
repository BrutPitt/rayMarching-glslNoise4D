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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glWindow.h"


float *texCoord;
float *vertexCoord;

///////////////////////////////////////////////////////////////////////////////
// UNCOMMENT for screen manipulato virtualGizmo3D 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// if you want use also the virtualGizmo3D, screen manipulator, uncomment this:

//#define GLAPP_USE_VIRTUALGIZMO

// Global variable or member class
#ifdef GLAPP_USE_VIRTUALGIZMO 
#else
/////////////////////////////////////////////////////////////////////////////
// For imGuIZMO, declare global variable or member class quaternion
    glm::quat qRot = glm::quat(1.f, 0.f, 0.f, 0.f);

/////////////////////////////////////////////////////////////////////////////
// two helper functions, not really necessary (but comfortable)
    void setRotation(const glm::quat &q) { qRot = q; }
    glm::quat& getRotation() { return qRot; }
#endif


using namespace glm;

void setView()
{

    vec3 povVec( 0.f, 0, 7.f);
    vec3 tgtVec( 0.f, 0, 0.f);


    theWnd->projectionMatrix = glm::perspective(glm::radians(30.0f),(float)theApp->GetWidth()/(float)theApp->GetHeight(),0.0f,30.0f);
    theWnd->modelMatrix      = glm::mat4(1.0f);
    theWnd->viewMatrix       = glm::lookAt( povVec,
                                            tgtVec,
                                            vec3(0.0f, 1.0f, 0.0f));
#ifdef GLAPP_USE_VIRTUALGIZMO 
     theWnd->getGizmo().applyTransform(theWnd->modelMatrix);
#endif

    theWnd->mvMatrix         = theWnd->viewMatrix * theWnd->modelMatrix;
    theWnd->mvpMatrix        = theWnd->projectionMatrix * theWnd->mvMatrix;
   
}


void setViewOrtho()
{

    theWnd->projectionMatrix = glm::ortho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0);
    theWnd->modelMatrix      = glm::mat4(1.0f);
    theWnd->viewMatrix       = glm::lookAt( vec3(0.f, 0.f, 1.f),
                                            vec3(0.f, 0.f, 0.f),
                                            vec3(0.f, 1.f, 0.f));

    theWnd->mvpMatrix        = theWnd->projectionMatrix * theWnd->viewMatrix * theWnd->modelMatrix;

}


void glWindow::BuildShader()
{

	// The brick shader classes are created here
	vertTexBackground = new VertexShader;
	fragTexBackground = new FragmentShader;
    //geomglslNoise = new GeometryShader;
	progTexBackground = new ProgramObject;
    progglslNoise = new ProgramObject;

	// The shader code is loaded and compiled here
	// Place the shader code where the executable is
	vertTexBackground->Load(SHADERS_PATH "vsTexBackground.glsl");
	fragTexBackground->Load(SHADERS_PATH "fsTexBackground.glsl");

	progTexBackground->addShader(vertTexBackground);
    progTexBackground->addShader(fragTexBackground);

    progTexBackground->link();

    progTexBackground->useProgram();

    progTexBackground->setUniform1i(progTexBackground->getUniformLocation("backgroundTex"),m_vR->getVolume()->getBackground());


    ProgramObject::reset();



/*
    geomglslNoise->Load("zzzGShader.glsl");

	// The vertex and fragment are added to the program object
    {
        glProgramParameteriEXT(progglslNoise->getHandle(),GL_GEOMETRY_INPUT_TYPE,GL_POINTS);
        glProgramParameteriEXT(progglslNoise->getHandle(),GL_GEOMETRY_OUTPUT_TYPE,GL_POINTS);  //GL_TRIANGLE_STRIP

        int temp;
	    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES,&temp);
        glProgramParameteriEXT(progglslNoise->getHandle(),GL_GEOMETRY_VERTICES_OUT,4);


    }
*/
    //progglslNoise->addShader(vertglslNoise);
	progglslNoise->addShader(noiseT.getFragment());

	// After adding all the vertex and fragments shaders the
	// program object must be linked before its able to be used
	progglslNoise->link();

	// Set the uniforms to be used. The program object will retain the
	// values set.
	progglslNoise->useProgram();


    noiseT.setSamplers(progglslNoise);

    ProgramObject::reset();


    m_vR->loadPrograms();

}




glWindow::glWindow()
{
    

}


glWindow::~glWindow()
{

}



//
/////////////////////////////////////////////////
void glWindow::onInit()
{

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background

    glViewport(0,0,theApp->GetWidth(), theApp->GetHeight());

#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    getGizmo().setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    getGizmo().setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    getGizmo().setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    getGizmo().setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    getGizmo().setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SHIFT);
    //getGizmo().setPanControls(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_SHIFT);

    // viewportSize  is need to set mouse sensitivity for rotation
    // You nedd to call it in your "reshape" function: when resize the window
    // look below
    getGizmo().viewportSize(theApp->GetWidth(), theApp->GetHeight());

    getGizmo().setDollyPosition(-5.0f);

    {
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 m(1.0f);
    getGizmo().applyTransform(m);
    glLoadMatrixf(glm::value_ptr(m));
    }

    {
    glMatrixMode(GL_PROJECTION);    
    glm::mat4 m = glm::perspective(glm::radians(40.0f), float(theApp->GetWidth())/float(theApp->GetHeight()), .1f, 10.0f);

    glLoadMatrixf(glm::value_ptr(m));
    }

    // If you need to more feeling with the mouse use:
    // getGizmo().setGizmoFeeling(1.0);
    // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible
    

    // other settings if you need it
    /////////////////////////////////////////////
    //getGizmo().setDollyScale(.001f);
    //getGizmo().setDollyPosition(1.0f);
    //getGizmo().setRotationCenter(vec3(0.0));
#endif


    //SetWindowTitle("glslNoise");
    //SetWindowSize(1024, 768);
    const int dim =256;
    m_vB = new VolumeBuffer(GL_LUMINANCE, dim, dim, dim, 1);
    m_vR = new VolumeRender(m_vB);

    noiseT.genNoiseTex4D();
    BuildShader();

}



//
/////////////////////////////////////////////////
void glWindow::onExit()
{
	delete vertTexBackground;
	delete fragTexBackground;
    delete progTexBackground;
	//delete geomglslNoise;

	delete progglslNoise;

    delete m_vB;
    delete m_vR;

    delete texCoord;
    delete vertexCoord;

    
}

void glWindow::DrawOnTexture()
{
    //PRINT_STATUS("BEGIN : HelloGPGPU::update()");
    //glGetIntegerv(GL_DRAW_BUFFER, &_currentDrawbuf); // Save the current Draw buffer
    								 // Render to the FBO
    //PRINT_STATUS("HelloGPGPU::update() : After FBO setup");


    // store the window viewport dimensions so we can reset them,
    // and set the viewport to the dimensions of our texture
	// Save the view port and set it to the size of the texture
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, GetWidth(), GetHeight());

    glDrawBuffer(GL_COLOR_ATTACHMENT0);			 // Draw into the first texture


    // GPGPU CONCEPT 3a: One-to-one Pixel to Texel Mapping: A Data-
    //                   Dimensioned Viewport.
    // We need a one-to-one mapping of pixels to texels in order to
    // ensure every element of our texture is processed. By setting our
    // viewport to the dimensions of our destination texture and drawing
    // a screen-sized quad (see below), we ensure that every pixel of our
    // texel is generated and processed in the fragment program.
    //glViewport(0, 0, getWidth(), getHeight());
    //glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();

    glPopAttrib();


    //RenderingOnTexture


	
    //PRINT_STATUS("END : HelloGPGPU::update()");

    


}


//
/////////////////////////////////////////////////
float timeT = 0.0;

void glWindow::onRender()
{

    if(!pause) {
////////////////////////////////////
// 4D set environmebt & FBO
////////////////////////////////////
    glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT); //GL_ALL_ATTRIB_BITS GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT
    m_vB->bind();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, m_vB->getWidth(), m_vB->getHeight());


    timeT+=.01;

    //glViewport(0, 0, 512, 512);

	progglslNoise->useProgram();
    noiseT.bindTextures();

    glUniform1f(noiseT.getTimeLoc(),timeT);

////////////////////////////////////
// 4D slice draw
////////////////////////////////////

    for(int z=0; z<m_vB->getDepth(); z++) {


        // attach texture slice to FBO
        m_vB->getFBO()->AttachTexture(GL_TEXTURE_3D, m_vB->getTexture(), GL_COLOR_ATTACHMENT0, 0, z);
        CHECK_GL_ERROR();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render
        float zi = (z + 0.5f) / (float) m_vB->getDepth();

        m_vB->drawSlice(zi);

        CHECK_GL_ERROR();
    }
    

    ProgramObject::reset();

////////////////////////////////////
// 4D restore enviroment
////////////////////////////////////

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    m_vB->unbind();



    glPopAttrib();

    }
    

////////////////////////////////////
// BackGround
////////////////////////////////////

//    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////////////////////////////////////
// 4D rayMarch set
////////////////////////////////////

    //glEnable(GL_DEPTH_TEST);
/*
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glClear(GL_COLOR_BUFFER_BIT);

    CHECK_GL_ERROR();
*/
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    progTexBackground->useProgram();
    glActiveTexture(GL_TEXTURE0+m_vR->getVolume()->getBackground());
    glBindTexture(GL_TEXTURE_2D, m_vR->getVolume()->getBackground());

    glBegin( GL_QUADS );
        glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.f);
        glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.f);
        glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.f);
        glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.f);
    glEnd();
    ProgramObject::reset();


    glm::mat4 m(1.0f);
    getGizmo().applyTransform(m);
    glLoadMatrixf(glm::value_ptr(m));

    m_vR->render();

    //progTexBackground->select();

    //glViewport(0, 0, 512, 512);

    //glEnable(GL_TEXTURE_2D);
    
    progTexBackground->useProgram();

    ProgramObject::reset();
  
    //int *p = nullptr;
    m_vR->WireCube(2.0f);
    
    glTranslatef(1.1,1.1,1.1);
    glColor3f(1.0,1.0,0.0);
    m_vR->WireSphere(.025, 12, 6);

    glColor3f(1.0,1.0,1.0);
/*
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
*/
}



//
/////////////////////////////////////////////////
void glWindow::onIdle()
{
#ifdef GLAPP_USE_VIRTUALGIZMO 
    //getGizmo().idle();
#endif
}


//
/////////////////////////////////////////////////
void glWindow::onReshape(GLint w, GLint h)
{
    glViewport(0,0,w,h);
    theApp->SetWidth(w); theApp->SetHeight(h);

#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().viewportSize(w, h);
#endif

    glMatrixMode(GL_PROJECTION);
    
    //glOrtho(-2.0, 2.0, -2.0, 2.0, -.1, 10.0);
    glm::mat4 m = glm::perspective(glm::radians(40.0f), float(theApp->GetWidth())/float(theApp->GetHeight()), .1f, 10.0f);

    glLoadMatrixf(glm::value_ptr(m));
    
}

//
/////////////////////////////////////////////////
void glWindow::onKeyUp(unsigned char key, int x, int y)
{

}


//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyDown(int key, int x, int y)
{


}


//
/////////////////////////////////////////////////
void glWindow::onKeyDown(unsigned char key, int x, int y)
{
    if(key == 13) m_vR->toggleTransparent();


}



//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyUp(int key, int x, int y)
{



}


//
/////////////////////////////////////////////////
void glWindow::onMouseButton(int button, int upOrDown, int x, int y)
{

#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().mouse((vgButtons) (button),
                    (vgModifiers) theApp->getModifier(),
                    upOrDown==GLFW_PRESS, x, y);
#endif

}

//
/////////////////////////////////////////////////
void glWindow::onMouseWheel(int wheel, int direction, int x, int y)
{

}

//
/////////////////////////////////////////////////
void glWindow::onMotion(int x, int y)
{
#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().motion(x, y);
#endif

    //qjSet->matOrientation *= trackball.getDollyPosition();
    
}

//
/////////////////////////////////////////////////
void glWindow::onPassiveMotion(int x, int y)
{

}
