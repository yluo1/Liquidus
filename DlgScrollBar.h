#ifndef _DLGSCROLLBAR_
#define _DLGSCROLLBAR_

#include "ConstantsIO.h"
#include "Dlg.h"
#include "Mouse.h"
#include "VectorT.h"

#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"


class DlgScrollBar:public Dlg{
private:
	Mouse *buttonMouse;
	bool mouseOver;
	bool mouseClicked;
	bool mouseDragged;

	VectorT range;

	VectorT pivotPosition;
	VectorT pivot[4];
	VectorT lastMousePosition;
	VectorT clickRelativePivot;

	float currentValue;
	float maxValue;
	bool horizontal;
public:

	DlgScrollBar():Dlg(){
	}

	DlgScrollBar(VectorT position,VectorT dims,VectorT range,bool horizontal,
		VectorT color,GLint texID,char *dlgString,bool active,bool visible,
		Mouse *buttonMouse)
		:Dlg(position,dims,color,texID,dlgString,active,visible){
		this->buttonMouse=buttonMouse;
		this->range=range;
		this->horizontal=horizontal;
		mouseOver=false;
		mouseClicked=false;
		mouseDragged=false;
		currentValue=(range.x+range.y)/2;
		maxValue=fabs(range.x-range.y);

		updatePivot(currentValue);

	}	

	virtual ~DlgScrollBar(){
	}

	virtual void display(){

		if(visible){
			glColor4fv((GLfloat*)&color);

			glPushAttrib(GL_ENABLE_BIT);

			if(dlgString!=NULL){	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				string s=dlgString;
				s.append(numberToString(currentValue/maxValue*255));
				defaultDisplay.displayString(s,position.x,position.y,.08,NULL);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}		
			if(horizontal){
				glBegin(GL_LINES);
				glVertex2f(position.x,position.y);
				glVertex2f(position.x,position.y+dims.y);
				
				glVertex2f(position.x+dims.x,position.y);
				glVertex2f(position.x+dims.x,position.y+dims.y);

				glVertex2f(position.x,position.y+dims.y/2);
				glVertex2f(position.x+dims.x,position.y+dims.y/2);
				glEnd();

				glBegin(GL_QUADS);
				glVertex3fv((GLfloat*)pivot);
				glVertex3fv((GLfloat*)(pivot+1));
				glVertex3fv((GLfloat*)(pivot+2));
				glVertex3fv((GLfloat*)(pivot+3));
				glEnd();
				
			}else{
			
			}


			glPopAttrib();
		}
	}

	virtual void display(float alpha){

		if(visible){
			glColor4f(color.x,color.y,color.z,alpha);

			glPushAttrib(GL_ENABLE_BIT);

			if(dlgString!=NULL){	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				string s=dlgString;
				s.append(numberToString(currentValue/maxValue*255));
				defaultDisplay.displayString(s,position.x,position.y,.08,NULL);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}		
			if(horizontal){
				glBegin(GL_LINES);
				glVertex2f(position.x,position.y);
				glVertex2f(position.x,position.y+dims.y);
				
				glVertex2f(position.x+dims.x,position.y);
				glVertex2f(position.x+dims.x,position.y+dims.y);

				glVertex2f(position.x,position.y+dims.y/2);
				glVertex2f(position.x+dims.x,position.y+dims.y/2);
				glEnd();

				glBegin(GL_QUADS);
				glVertex3fv((GLfloat*)pivot);
				glVertex3fv((GLfloat*)(pivot+1));
				glVertex3fv((GLfloat*)(pivot+2));
				glVertex3fv((GLfloat*)(pivot+3));
				glEnd();
				
			}else{
			
			}

			glPopAttrib();
		}
	}

	virtual void update(){
		mouseClicked=false;
		if(active){
			VectorT mousePosition=buttonMouse->position;
			if(mousePosition.x>=pivot[0].x&&mousePosition.x<=pivot[1].x&&mousePosition.y>=pivot[2].y&&mousePosition.y<=pivot[0].y){
				mouseOver=true;
				if(buttonMouse->mouseState[0]&&!buttonMouse->lastMouseState[0]){
					buttonMouse->lastMouseState[0]=1;
					lastMousePosition=buttonMouse->position;
					clickRelativePivot=buttonMouse->position-pivotPosition;
					mouseDragged=true;
					mouseClicked=true;
				}

			}else mouseOver=false;

			if(!buttonMouse->mouseState[0]){
				mouseDragged=false;
			}

		}else{
			mouseOver=false;
			mouseClicked=false;
		}
		

		if(mouseDragged){
			if(horizontal){

				float deltaMove=(buttonMouse->position.x-lastMousePosition.x)*(1/dims.x);
				if(deltaMove>0){
					if(buttonMouse->position.x<pivotPosition.x+clickRelativePivot.x) deltaMove=0;
				}else{
					if(buttonMouse->position.x>pivotPosition.x+clickRelativePivot.x) deltaMove=0;
				}
				currentValue+=deltaMove;
				currentValue=maxValue(minValue(currentValue,range.y),range.x);
				updatePivot(currentValue);
				lastMousePosition=buttonMouse->position;
			}
		}

	}

	void updatePivot(float currentValue){
		pivotPosition=VectorT(position.x+dims.x*(currentValue/maxValue),position.y+dims.y/2);
		pivot[0]=VectorT(pivotPosition.x-dims.x*.05,position.y+dims.y);
		pivot[1]=VectorT(pivotPosition.x+dims.x*.05,position.y+dims.y);
		pivot[2]=VectorT(pivotPosition.x+dims.x*.05,position.y);
		pivot[3]=VectorT(pivotPosition.x-dims.x*.05,position.y);
	}

	float getValue(){
		return currentValue;
	}

	bool getMouseClicked(){
		return mouseClicked;
	}

	virtual void resize(VectorT windowDims,VectorT oldDims){
		Dlg::resize(windowDims,oldDims);
		updatePivot(currentValue);
	}
};

#endif