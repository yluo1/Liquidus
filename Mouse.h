#ifndef _MOUSE_
#define _MOUSE_

#include <stdio.h>
#include "Constants.h"
#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"
#include "VectorT.h"

////////////////////////////////////////////////

class Mouse{
private:
	bool warpOn;	
public:
	bool mouseOnOff;



	float sensitivity;

	int mouseState[5];
	int lastMouseState[5];
	VectorT clickOrigin[5];

	VectorT position;
	VectorT lastPosition;
	VectorT center;
	VectorT distanceMoved;

	int mouseMovementMode;//mouse movement mode (0=slide, 1=fps)

	Mouse(){
		warpOn=false;
		center=VectorT(WINDOWWIDTH/2,WINDOWHEIGHT/2);
		sensitivity=.0002;
		mouseOnOff=true;
		for(int a=0;a<5;++a){
			mouseState[a]=lastMouseState[a]=0;
		}
		mouseMovementMode=0;
	}

	//udpate mouse functions
	void changeMouseCords(int x1,int y1,int button,int state){
		if(mouseOnOff){
			for(int a=0;a<5;++a) lastMouseState[a]=mouseState[a];

			mouseState[button]=!state;
			if(!state) clickOrigin[button]=VectorT(x1,y1,0.0f);
			lastPosition=position;
			position=VectorT(x1,y1);
			distanceMoved=position-lastPosition;
		
			if(mouseMovementMode==1){
				if(warpOn==false){
					warpOn=true;
					glutWarpPointer(center.x,center.y);	
				return;
				}
			}
			warpOn=false;
		}
	}
	void changeMouseCords(int x1,int y1){
		if(mouseOnOff){
			//update mouse coordinates
			lastPosition=position;
			position=VectorT(x1,y1);
			distanceMoved=position-lastPosition;

			if(mouseMovementMode==1){
				if(warpOn==false){
					warpOn=true;
					glutWarpPointer(center.x,center.y);	
					return;
				}
				warpOn=false;
			}
		}
	}
	void changeMouseMovementMode(int mode){
		
		mouseMovementMode=mode;
		if(mouseMovementMode==0) glutSetCursor(GLUT_CURSOR_INHERIT); 
		else if(mouseMovementMode==1){
		glutSetCursor(GLUT_CURSOR_NONE); 
		#ifdef __CARBON__
			CGSetLocalEventsSuppressionInterval(0);
		#endif
		}
	}
};


#endif