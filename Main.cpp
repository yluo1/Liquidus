////////////////////////////////////////////////Libraries
//Debug info
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "Constants.h"
#include "Display.h"
#include "Extensions.h"
#include "ForcePong.h"
#include "ConstantsIO.h"
#include "Conversion.h"
#include "Sound.h"
#include "VectorT.h"

void forIdle();
void forDisplay();

//keyboard functions
void processSpecialKeys(int key, int x, int y);
void processNormalKeys(unsigned char key, int x, int y);
void processSpecialKeysUp(int key, int x, int y);
void processNormalKeysUp(unsigned char key, int x, int y);

//mouse functions
void processMouse(int button, int state, int x, int y);//controls movement with passive and active mouse
void processMousePassiveMotion(int x, int y);
void processMouseActiveMotion(int x, int y);

//Main functions
void Runner();
void init();

//Secondary functions
void cleanup();
void changeSize(int width,int height);
void setVsync(bool vSyncOn);

//Etc
float frameRate=0.0f;
float pTime=glutGet(GLUT_ELAPSED_TIME);
float fTime=pTime;
int frameCount=0;

//Game
ForcePong forcePong;

////////////////////////////////////////////////Main loop entry point
int main(int argc, char** argv){//main function
	//Debug info
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	atexit(cleanup);

	glutInit(&argc,argv);
	//Window functions
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	

	if(FULLSCREEN){
	//Attempt to enter gamemode
		glutGameModeString("1024x768:32");
		glutEnterGameMode();   
	}else{
		glutInitWindowSize(WINDOWWIDTH,WINDOWHEIGHT);
		glutCreateWindow("Liquidus");  
	}

	glutDisplayFunc(forDisplay);  
	glutIdleFunc(forIdle);


	//keyboard functions
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutSpecialUpFunc(processSpecialKeysUp);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); 
	//Mouse functions
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutReshapeFunc(changeSize);

	GLenum err = glewInit();
	if (GLEW_OK != err){
	  printf("Error: %s\n", glewGetErrorString(err));
	}
	if(GL_TRUE != glewGetExtension( "GL_ARB_fragment_shader"))
	   printf("GL_ARB_fragment_shader extension is not available!");
	if(GL_TRUE != glewGetExtension("GL_ARB_vertex_shader"))
	   printf("GL_ARB_vertex_shader extension is not available!");
	if(GL_TRUE != glewGetExtension("GL_ARB_shader_objects"))
	  printf("GL_ARB_shader_objects extension is not available!");

	if( ! GLEW_ATI_texture_float )
		printf("NO GLEW_ATI_texture_float");
	
	//Sound
	//Initialize the OpenAL library
	Sound::init(argc,argv);
	init();
	glutMainLoop(); 
	return 1;
}//end of function


void forIdle(){
	float currentTime=glutGet(GLUT_ELAPSED_TIME);
	//Frame caps
	if(currentTime-fTime>=1000/GAMESPEED){
		fTime=currentTime;
		if(currentTime-pTime>=1000){
			pTime=currentTime;
			frameRate=frameCount;//calc framerates
			frameCount=0;	
		}
		++frameCount;
		Runner();
	}

	glutSwapBuffers();
	glutPostRedisplay();	
}

void cleanup(){
	forcePong.cleanup();
	delete []defaultSinValues;
	Sound::kill();
}


void forDisplay(){

}


void Runner(){
	forcePong.run();

	string fpsString=numberToString((int)frameRate);
	glColor3f(0,0,0);
	defaultDisplay.displayString(fpsString,defaultDisplay.windowWidth-100,50,FONT_SIZE,NULL);
}

void init(){
	setVsync(VSYNC_ON);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);


	forcePong.init();
}


void processSpecialKeys(int key, int x, int y){
//catch special keys and store in static keyboard variables.
	defaultMouse.changeMouseCords(x,y);
	defaultKeyboard.update(key,1);

}

void processNormalKeys(unsigned char key, int x, int y) {
	defaultMouse.changeMouseCords(x,y);
	defaultKeyboard.update(key,1);
}

void processSpecialKeysUp(int key, int x, int y){
	defaultMouse.changeMouseCords(x,y);
	defaultKeyboard.update(key,0);
}

void processNormalKeysUp(unsigned char key, int x, int y){
	defaultMouse.changeMouseCords(x,y);
	defaultKeyboard.update(key,0);
}

void processMouseActiveMotion(int x, int y){
	defaultMouse.changeMouseCords(x,y);
}

void processMouse(int button, int state, int x, int y){
	defaultMouse.changeMouseCords(x,y,button,state);
}

void processMousePassiveMotion(int x, int y){
	defaultMouse.changeMouseCords(x,y);
}


/////////////////////////////////////////////////
//Secondary Functions
////////////////////////////////////////////////

void changeSize(int width,int height){
	forcePong.resize(VectorT(width,height),VectorT(defaultDisplay.windowWidth,defaultDisplay.windowHeight));
	defaultDisplay.changeSize(width,height);

}
void setVsync(bool vSyncOn){
	  const GLubyte *extensions = glGetString( GL_EXTENSIONS );

	  if( strstr((char*)extensions, "WGL_EXT_swap_control" ) == 0 )
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	  else
	  {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if( wglSwapIntervalEXT )
		  wglSwapIntervalEXT(vSyncOn);
	  }
}


