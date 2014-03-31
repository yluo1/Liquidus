#ifndef _DISPLAY_
#define _DISPLAY_

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "GL/glew.h"
#include "GL/glut.h"

#include "Constants.h"
#include "VectorT.h"

using namespace std;

class Display{
private:

public:
	int windowWidth,windowHeight;
	int defaultWidth,defaultHeight;
	float minClipDistance,maxClipDistance;
	float fieldOfView;
	float windowScaleX,windowScaleY;

	bool wireFrameOn;
	bool displayMode;

	VectorT cameraPosition;
	VectorT cameraTarget;

	Display(){
	
	}

	Display(int windowWidth,int windowHeight,float minClipDistance,float maxClipDistance,float fieldOfView){
		this->windowWidth=windowWidth;
		this->windowHeight=windowHeight;
		this->minClipDistance=minClipDistance;
		this->maxClipDistance=maxClipDistance;
		this->fieldOfView=fieldOfView;
		defaultWidth=1024;
		defaultHeight=768;
		windowScaleX=(float)windowWidth/defaultWidth;
		windowScaleY=(float)windowHeight/defaultHeight;
		wireFrameOn=false;
	}

	void initModelCamera3D(){	
		displayMode=DISPLAY_3D;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0,0,windowWidth, windowHeight);
		gluPerspective(fieldOfView,(float)windowWidth/windowHeight,minClipDistance,maxClipDistance);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_NORMALIZE);	

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
		if(wireFrameOn) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glShadeModel(GL_SMOOTH);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);

		glMatrixMode(GL_MODELVIEW);	

	}

	void initModelCamera2D(){
		displayMode=DISPLAY_2D;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0,0,windowWidth, windowHeight);
		glOrtho(0.0f,windowWidth,windowHeight,0.0f,-1.0f,1.0f);
	
		glMatrixMode(GL_MODELVIEW);	
	}

	void updateModelCamera3D(){
		gluLookAt(cameraPosition.x,cameraPosition.y,cameraPosition.z,
					cameraTarget.x,cameraTarget.y,cameraTarget.z,
					0,1,0);
	}

	void displayString(string text, int x,int y,float size,void *font){
		if(displayMode!=DISPLAY_2D) initModelCamera2D();

		int newLines=0;
		if(font==NULL) font=GLUT_STROKE_ROMAN;
		glPushMatrix();
		glTranslatef(x,y,0);
		glScalef(size,-size,1);
		for(int a=0;a<(int)text.length();++a){
			if(text[a]=='\n'){
				glPopMatrix();
				glPushMatrix();
				glTranslatef(x,y+size/.1*20*(++newLines),0);
				glScalef(size,-size,1);				
			}
			glutStrokeCharacter(font ,text[a]);
		}
		glPopMatrix();
		if(displayMode==DISPLAY_3D) initModelCamera3D();
	}
	
	void changeSize(int width,int height){
		windowWidth=width;
		windowHeight=height;
		if(displayMode==DISPLAY_2D) initModelCamera2D();
		if(displayMode==DISPLAY_3D) initModelCamera3D();
		windowScaleX=(float)windowWidth/defaultWidth;
		windowScaleY=(float)windowHeight/defaultHeight;
	}
};

#endif