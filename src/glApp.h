#pragma once
#include "appDefines.h"

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <GLFW/glfw3.h>
#ifdef GLAPP_USE_IMGUI
#include "ui\uiMainDlg.h"
#endif



#ifndef theApp
    #define theApp mainGLApp::theMainApp
#endif
#ifndef theWnd
    #define theWnd theApp->getEngineWnd()
#endif
#ifndef theDlg
    #define theDlg theApp->getMainDlg()
#endif


enum ScreeShotReq {
    ScrnSht_NO_REQUEST,
    ScrnSht_SILENT_MODE,
    ScrnSht_FILE_NAME
};


class glWindow;

void setGUIStyle();



/////////////////////////////////////////////////
// theApp -> Main App -> container
/////////////////////////////////////////////////


class mainGLApp
{
public:
    // self pointer .. static -> the only one 
    static mainGLApp* theMainApp;

    mainGLApp();
    ~mainGLApp();

    void onInit();
    int onExit();

    void mainLoop();
////////////////////////////////
//GLFW Utils
    GLFWwindow* getGLFWWnd()  const { return(mainGLFWwnd);  }

    glWindow *getEngineWnd() { return glEngineWnd; }
    void setGLFWWnd(GLFWwindow* wnd) { mainGLFWwnd = wnd; }

	int getXPosition() const { return(xPosition); }
    int getYPosition() const { return(yPosition); }
	int GetWidth()     const { return(width);     }
	int GetHeight()    const { return(height);    }
    void SetWidth(int v)  { width  = v; }
    void SetHeight(int v) { height = v; }
	const char* getWindowTitle() const { return(windowTitle.c_str()); }

protected:

		// The Position of the window
		int xPosition, yPosition;
		int width, height;
		/** The title of the window */

        bool exitFullScreen;

		// The title of the window
		std::string windowTitle;
    
private:
// imGui utils
/////////////////////////////////////////////////
#ifdef GLAPP_USE_IMGUI
    mainImGuiDlgClass mainImGuiDlg;
public:
    mainImGuiDlgClass &getMainDlg() { return mainImGuiDlg; }
#endif
private:

// glfw utils
/////////////////////////////////////////////////
    void glfwInit();
    int glfwExit();
    int getModifier();

    GLFWwindow* mainGLFWwnd;
    glWindow *glEngineWnd;

friend class glWindow;

};



