#ifndef _DLG_
#define _DLG_


#include <stdio.h>
#include <stdlib.h>

#include "GL/glew.h"
#include "GL/glut.h"

#include "ConstantsIO.h"
#include "VectorT.h"

#define TYPE_DLG		0
#define TYPE_DLG_BUTTON	1

/////////////////////
//Dlg the base dialogue box
//
//
//
/////////////////////
class Dlg{
protected:
	VectorT position;
	VectorT dims;
	VectorT color;
	VectorT initialColor;
	Dlg **dlgAddons;
	int nDlgAddons;

	char *dlgString;
	GLint texID;
	bool active;
	bool visible;
public:
	Dlg(){
		position=VectorT();
		dims=VectorT();
		color=VectorT();
		initialColor=VectorT();
		texID=-1;
		visible=false;
		active=false;
		dlgAddons=NULL;
		nDlgAddons=0;
		dlgString=NULL;
	}

	/*
	Arguments:	
		VectorT position: Top left point of the Dlg
		VectorT dims: Length and width dimensions of Dlg
		VectorT color: RGBA format
		GLint texID: textureID to be mapped, -1 to disable
		bool visible: true for visible, false for invisible
	Description:
		Base class for Dlg
	*/
	Dlg(VectorT position,VectorT dims,VectorT color,GLint texID,char *dlgString,bool active,bool visible){
		this->position=position;
		this->dims=dims;
		this->color=color;
		this->texID=texID;
		this->active=active;
		this->visible=visible;
		initialColor=color;
		this->color.w=0;
		this->dlgString=dlgString;
		dlgAddons=NULL;
		nDlgAddons=0;
	}

	virtual ~Dlg(){

	}

	virtual void display(){
		//Display will always display it but alpha will vary by visible or not

		glColor4fv((GLfloat*)&color);
		glPushAttrib(GL_ENABLE_BIT);

		if(dlgString!=NULL){	
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			defaultDisplay.displayString(dlgString,position.x,position.y,.125,NULL);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}	

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
		if(visible){
			for(int a=0;a<nDlgAddons;++a){
				if(dlgAddons[a]!=NULL) dlgAddons[a]->display();
			}
		}	
		glPopAttrib();
	}

	virtual void update(){
		if(active){
			for(int a=0;a<nDlgAddons;++a){
				if(dlgAddons[a]!=NULL){
					dlgAddons[a]->setActive(true);
					dlgAddons[a]->update();
				}
			}
		}else{
			for(int a=0;a<nDlgAddons;++a){
				if(dlgAddons[a]!=NULL){
					dlgAddons[a]->setActive(false);
				}
			}
		}

		if(visible) color+=(initialColor-color)*.02;
		else color*=.9;
		
	}

	void add(Dlg *dlgObject){
		if(dlgObject==NULL) return;
		for(int a=0;a<nDlgAddons;++a){
			if(dlgAddons[a]==NULL){//Add object
				dlgAddons[a]=dlgObject;
				return;
			}
		}
		//Expand dlgAddons
		nDlgAddons=nDlgAddons*2+(!nDlgAddons);
		dlgAddons=(Dlg**)realloc(dlgAddons,nDlgAddons*sizeof(Dlg));
		
		for(int a=nDlgAddons/2;a<nDlgAddons;++a) dlgAddons[a]=NULL;
		add(dlgObject);
	}

	void remove(Dlg *dlgObject){
		if(dlgObject==NULL) return;

		for(int a=0;a<nDlgAddons;++a){
			if(dlgAddons[a]==dlgObject) dlgAddons[a]=NULL;
		}
	}

	void remove(int dlgIndex){
		if(dlgIndex<nDlgAddons) dlgAddons[dlgIndex]=NULL;

	}

	void setActive(bool active){
		this->active=active;
	}

	void setVisible(bool visible){
		this->visible=visible;
	}

	bool getActive(){
		return active;
	}
	bool getVisible(){
		return visible;
	}
	VectorT getPosition(){
		return position;
	}

	virtual void resize(VectorT windowDims,VectorT oldDims){
		position.x*=windowDims.x/oldDims.x;
		position.y*=windowDims.y/oldDims.y;

		dims.x*=windowDims.x/oldDims.x;
		dims.y*=windowDims.y/oldDims.y;
	}

	virtual void cleanup(){
		free(dlgAddons);
	}

};

#endif