#ifndef _DLGCOLORPICKER_
#define _DLGCOLORPICKER_

#include "ConstantsIO.h"
#include "Dlg.h"
#include "DlgScrollBar.h"
#include "Mouse.h"
#include "VectorT.h"

#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"


class DlgColorPicker:public Dlg{
private:
	Mouse *buttonMouse;
	bool mouseOver;
	bool mouseClicked;
	Texture colorPalette;

	DlgScrollBar valueBar;
	float *paletteArray;
	float value;

	VectorT currentColor;
public:

	DlgColorPicker():Dlg(){
	}

	DlgColorPicker(VectorT position,VectorT dims,VectorT color,GLint texID,char *dlgString,bool active,bool visible,
		Mouse *buttonMouse)
		:Dlg(position,dims,color,texID,dlgString,active,visible){
		this->buttonMouse=buttonMouse;
		mouseOver=false;
		mouseClicked=false;

		paletteArray=makeHSVPalette(dims.x,dims.y);
		
		colorPalette.makeTexture(paletteArray,dims.x,dims.y,GL_RGBA,GL_RGBA,GL_FLOAT);
		valueBar=DlgScrollBar(VectorT(position.x,position.y+dims.y*1.05),VectorT(dims.x,dims.y*.1),VectorT(0,1),true,color,-1,"Value:",active,visible,buttonMouse);
		currentColor=VectorT(1,0,0);
	}

	virtual ~DlgColorPicker(){
	}

	virtual void display(){

		if(visible){
			glColor4fv((GLfloat*)&color);

			glPushAttrib(GL_ENABLE_BIT);

			if(dlgString!=NULL){	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				defaultDisplay.displayString(dlgString,position.x,position.y,.125,NULL);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}	

			//Draw color palette
			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,colorPalette.texID);
			glTranslatef(position.x,position.y,0);
			glBegin(GL_QUADS);
			glTexCoord2d(0,0);		glVertex2f(0,0);
			glTexCoord2d(1.0,0);	glVertex2f(dims.x,0);
			glTexCoord2d(1.0,1.0);	glVertex2f(dims.x,dims.y);
			glTexCoord2d(0,1.0);	glVertex2f(0,dims.y);
			glEnd();
			
			glDisable(GL_TEXTURE_2D);

			if(value<.5)glColor4f(0,0,0,(.5-value)*2);
			else glColor4f(1,1,1,(value-.5)*2);
	
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(dims.x,0);
			glVertex2f(dims.x,dims.y);
			glVertex2f(0,dims.y);
			glEnd();

			glPopMatrix();

			valueBar.display();

			//Draw current color
			glColor4f(currentColor.x,currentColor.y,currentColor.z,color.w);
			glPushMatrix();
			glTranslatef(position.x,position.y+dims.y*1.2,0);
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(dims.x*.15,0);
			glVertex2f(dims.x*.15,dims.y*.1);
			glVertex2f(0,dims.y*.1);
			glEnd();

			glPopAttrib();
		}
	}

	virtual void display(float alpha){

		if(visible){
			glColor4f(color.x,color.y,color.z,alpha);

			glPushAttrib(GL_ENABLE_BIT);

			if(dlgString!=NULL){	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				defaultDisplay.displayString(dlgString,position.x,position.y,.125,NULL);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}	

			//Draw color palette
			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,colorPalette.texID);
			glTranslatef(position.x,position.y,0);
			glBegin(GL_QUADS);
			glTexCoord2d(0,0);		glVertex2f(0,0);
			glTexCoord2d(1.0,0);	glVertex2f(dims.x,0);
			glTexCoord2d(1.0,1.0);	glVertex2f(dims.x,dims.y);
			glTexCoord2d(0,1.0);	glVertex2f(0,dims.y);
			glEnd();
			
			glDisable(GL_TEXTURE_2D);

			if(value<.5)glColor4f(0,0,0,(.5-value)*2*alpha);
			else glColor4f(1,1,1,(value-.5)*2*alpha);
	
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(dims.x,0);
			glVertex2f(dims.x,dims.y);
			glVertex2f(0,dims.y);
			glEnd();

			glPopMatrix();

			//Display value bar
			valueBar.display(alpha);

			//Draw current color
			glColor4f(currentColor.x,currentColor.y,currentColor.z,alpha);
			glPushMatrix();
			glTranslatef(position.x,position.y+dims.y*1.2,0);
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(dims.x*.15,0);
			glVertex2f(dims.x*.15,dims.y*.1);
			glVertex2f(0,dims.y*.1);
			glEnd();

			glPopMatrix();


			glPopAttrib();
		}
	}

	virtual void update(){

		if(active){
			valueBar.update();
			value=valueBar.getValue();

			VectorT mousePosition=buttonMouse->position;
			if(mousePosition.x>=position.x&&mousePosition.x<=position.x+dims.x&&mousePosition.y>=position.y&&mousePosition.y<=position.y+dims.y){
				mouseOver=true;
				if(buttonMouse->mouseState[0]&&!buttonMouse->lastMouseState[0]){
					buttonMouse->lastMouseState[0]=1;
					mouseClicked=true;
				}
			}else mouseOver=false;

			if(!buttonMouse->mouseState[0]) mouseClicked=false;
		}else{
			mouseOver=false;
			mouseClicked=false;
		}

		//Get Selected color
		if(mouseClicked){
			VectorT cPos=buttonMouse->position-position;
			int ref=4*(int)(maxValue(minValue(cPos.y,dims.y-1),0)*dims.x+maxValue(minValue(cPos.x,dims.x-1),0));
			currentColor=VectorT(paletteArray[ref],
								paletteArray[ref+1],
								paletteArray[ref+2],
								1);
			//Modify color by value
			if(value<.5) currentColor-=currentColor*(.5-value)*2;
			else currentColor+=(-currentColor+1)*(value-.5)*2;
			currentColor.w=1.0;
			
		}
		
	}

	float* makeHSVPalette(int width,int height){
		VectorT hsv;
		VectorT rgb;
		hsv.y=.5;
		hsv.z=1.0;
		float *imageDataFloat=new float[width*height*4];
		for(int a=0;a<height;++a){
			hsv.y=(float)a/height;
			for(int b=0;b<width;++b){
				hsv.x=360.0/(width)*b;
				rgb=HSVToRGB(hsv);
				imageDataFloat[4*(a*width+b)+0]=rgb.x;
				imageDataFloat[4*(a*width+b)+1]=rgb.y;
				imageDataFloat[4*(a*width+b)+2]=rgb.z;
				imageDataFloat[4*(a*width+b)+3]=1.0f;

			}
		}
		return imageDataFloat;
	}

	VectorT getCurrentColor(){
		return currentColor;
	}

	bool getMouseClicked(){
		return mouseClicked;
	}

	virtual void resize(VectorT windowDims,VectorT oldDims){
		Dlg::resize(windowDims,oldDims);
		valueBar.resize(windowDims,oldDims);
	}
	
	virtual void cleanup(){
		Dlg::cleanup();

		free(paletteArray);
	}

};

#endif