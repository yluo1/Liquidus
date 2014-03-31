#ifndef _DLGBUTTON_
#define _DLGBUTTON_

#include "ConstantsIO.h"
#include "Dlg.h"
#include "Mouse.h"
#include "VectorT.h"

#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"

/////////////////////
//Dlg is a static pannel
//
//
//
/////////////////////
class DlgButton:public Dlg{
private:
	Mouse *buttonMouse;
	bool mouseOver;
	bool mouseClicked;
public:

	DlgButton():Dlg(){
	}
	DlgButton(VectorT position,VectorT dims,VectorT color,GLint texID,char *dlgString,bool active,bool visible,
		Mouse *buttonMouse)
		:Dlg(position,dims,color,texID,dlgString,active,visible){
		this->buttonMouse=buttonMouse;
		mouseOver=false;
		mouseClicked=false;
		this->color.w*=.5;
	}

	virtual ~DlgButton(){
	}

	virtual void display(){

		if(visible){
			glColor4fv((GLfloat*)&color);

			glPushAttrib(GL_ENABLE_BIT);
			if(texID==-1){
				glDisable(GL_TEXTURE_2D);

				glBegin(GL_TRIANGLE_STRIP);
				glVertex2f(position.x,position.y);//bottom left
				glVertex2f(position.x,position.y+dims.y);//Top left
				glVertex2f(position.x+dims.x,position.y);//bottom right
				glVertex2f(position.x+dims.x,position.y+dims.y);//top right
				glEnd();
			}else{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,texID);

				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2d(0,1);
				glVertex2f(position.x,position.y);//bottom left

				glTexCoord2d(0,0);
				glVertex2f(position.x,position.y+dims.y);//Top left

				glTexCoord2d(1,1);
				glVertex2f(position.x+dims.x,position.y);//bottom right

				glTexCoord2d(1,0);
				glVertex2f(position.x+dims.x,position.y+dims.y);//top right
				glEnd();
			}
			glPopAttrib();
		}
	}

	virtual void update(){
		mouseClicked=false;
		if(active){
			VectorT mousePosition=buttonMouse->position;
			if(mousePosition.x>=position.x&&mousePosition.x<=position.x+dims.x&&mousePosition.y>=position.y&&mousePosition.y<=position.y+dims.y){
				mouseOver=true;
				if(buttonMouse->mouseState[0]&&!buttonMouse->lastMouseState[0]){
					buttonMouse->lastMouseState[0]=1;
					mouseClicked=true;
				}
			}else mouseOver=false;
		}else{
			mouseOver=false;
			mouseClicked=false;
		}
		
		if(mouseOver) color+=(initialColor-color)*.1;
		else color+=(initialColor*.5-color)*.1;
	}

	bool getMouseClicked(){
		return mouseClicked;
	}

	bool getMouseOver(){
		return mouseOver;
	}

};

#endif