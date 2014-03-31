#ifndef _FORCEPONGSHIPEDITOR_
#define _FORCEPONGSHIPEDITOR_

#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"

#include "Display.h"
#include "Dlg.h"
#include "DlgColorPicker.h"
#include "DlgButton.h"
#include "Model.h"
#include "VectorT.h"
#include "Constants.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "StringBuffer.h"
#include "Texture.h"

#include "ForcePongConstants.h"


class ForcePongShipEditor:public Dlg{
private:
	Model currentModel;
	Mouse *currentMouse;
	Display *currentDisplay;

	StringBuffer statusBuffer;

	Model *userModel;
	
	DlgColorPicker colorPicker;
	VectorT currentColor;

	float constructionRadi;
	VectorT constructionOrigin;

	VectorT constructionPts[3];
	int currentPt;

	bool mouseClicked;

	DlgButton setButton;
	DlgButton resetButton;
	DlgButton saveButton;
	DlgButton undoButton;
	DlgButton defaultButton;

public:
	ForcePongShipEditor():Dlg(){

	}

	ForcePongShipEditor(VectorT position,VectorT dims,VectorT color,GLint texID,char *dlgString,bool active,bool visible,Mouse *currentMouse, Display *currentDisplay,Model *userModel)
		:Dlg(position,dims,color,texID,dlgString,active,visible){
		this->currentMouse=currentMouse;
		this->currentDisplay=currentDisplay;
		this->userModel=userModel;
		constructionRadi=dims.x*.33;
		constructionOrigin=position+dims/2;
		currentPt=0;
		currentModel.readModel(userShipModelPath);
		
		setButton=DlgButton(VectorT(constructionOrigin.x+constructionRadi,position.y+dims.y*.1-14),VectorT(50,20),VectorT(1,0,0,.5),-1,NULL,true,true,currentMouse);
		resetButton=DlgButton(VectorT(constructionOrigin.x+constructionRadi,position.y+dims.y*.15-14),VectorT(50,20),VectorT(1,0,0,.5),-1,NULL,true,true,currentMouse);
		saveButton=DlgButton(VectorT(constructionOrigin.x+constructionRadi,position.y+dims.y*.2-14),VectorT(50,20),VectorT(1,0,0,.5),-1,NULL,true,true,currentMouse);
		undoButton=DlgButton(VectorT(constructionOrigin.x-constructionRadi,position.y+dims.y*.1-14),VectorT(50,20),VectorT(1,0,0,.5),-1,NULL,true,true,currentMouse);
		defaultButton=DlgButton(VectorT(constructionOrigin.x+constructionRadi,position.y+dims.y*.25-14),VectorT(50,20),VectorT(1,0,0,.5),-1,NULL,true,true,currentMouse);

		colorPicker=DlgColorPicker(VectorT(position.x-dims.x*.5,position.y+dims.y*.25),
									dims*.5,VectorT(1,1,1,1),-1,"Color Picker",true,true,currentMouse);

	}

	~ForcePongShipEditor(){
	}

	void display(){
		Dlg::display();
		glPushAttrib(GL_ENABLE_BIT);
	
		glColor4f(0,1,.5,color.w);

		//Draw text
		currentDisplay->displayString("Head",constructionOrigin.x+constructionRadi+10,constructionOrigin.y,.1,NULL);

		currentDisplay->displayString("Set",constructionOrigin.x+constructionRadi,position.y+dims.y*.1,.1,NULL);
		currentDisplay->displayString("Reset",constructionOrigin.x+constructionRadi,position.y+dims.y*.15,.1,NULL);
		currentDisplay->displayString("Save",constructionOrigin.x+constructionRadi,position.y+dims.y*.2,.1,NULL);
		currentDisplay->displayString("Default",constructionOrigin.x+constructionRadi,position.y+dims.y*.25,.1,NULL);

		currentDisplay->displayString("Undo",constructionOrigin.x-constructionRadi,position.y+dims.y*.1,.1,NULL);
		

		//Draw construction grid
		glBegin(GL_LINE_LOOP);
		for(int a=0;a<50;++a)
		glVertex2f(constructionOrigin.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/50*a)%SIN_TABLE_SIZE]*constructionRadi,
				   constructionOrigin.y+defaultSinValues[SIN_TABLE_SIZE/50*a]*constructionRadi);
		glEnd();
		glBegin(GL_LINES);
		glVertex2f(constructionOrigin.x-dims.x*.4,constructionOrigin.y);
		glVertex2f(constructionOrigin.x+dims.x*.4,constructionOrigin.y);
		glVertex2f(constructionOrigin.x,constructionOrigin.y-dims.y*.4);
		glVertex2f(constructionOrigin.x,constructionOrigin.y+dims.y*.4);
		glEnd();

		//Draw models in construction region
		glPushMatrix();
		glTranslatef(constructionOrigin.x,constructionOrigin.y,0);
		
		//Draw current polygon
		for(int a=0;a<currentPt;++a){
			glBegin(GL_QUADS);
			glVertex2f(constructionPts[a].x+5,constructionPts[a].y+5);
			glVertex2f(constructionPts[a].x+5,constructionPts[a].y-5);
			glVertex2f(constructionPts[a].x-5,constructionPts[a].y-5);
			glVertex2f(constructionPts[a].x-5,constructionPts[a].y+5);
			glEnd();
		}
		glBegin(GL_LINE_STRIP);
		for(int a=0;a<currentPt;++a){
			glVertex2fv((GLfloat*)&constructionPts[a]);
		}
		glEnd();

		//Draw model
		glScalef(constructionRadi,constructionRadi,1);
		currentModel.display(.5*color.w);

		glPopMatrix();

		//Draw buttons
		if(active){
			setButton.display();
			resetButton.display();
			saveButton.display();
			undoButton.display();
			defaultButton.display();
		}

		//Draw color palette
		colorPicker.display(color.w);

		//Show status buffer
		statusBuffer.show();

		glPopAttrib();
	}

	void update(){
		Dlg::update();
		statusBuffer.update();

		mouseClicked=false;
		VectorT mousePosition=currentMouse->position;
		if(active){//Editor is active
			if((mousePosition-constructionOrigin).norm()<=constructionRadi){
				if(currentMouse->mouseState[0]&&!currentMouse->lastMouseState[0]){
					currentMouse->lastMouseState[0]=1;
					mouseClicked=true;
				}
			}

			//Update Buttons
			updateButtons();
			//Update Colorpicker
			colorPicker.update();
			currentColor=colorPicker.getCurrentColor();
			
		}else{
			mouseClicked=false;
		}
		
		if(mouseClicked){

			constructionPts[currentPt]=mousePosition-constructionOrigin;
			//Check for snap
			VectorT snapPoint;
			float d=currentModel.findClosestPoint(constructionPts[currentPt]/constructionRadi,&snapPoint);

			if(d<=.075){
				constructionPts[currentPt]=snapPoint*constructionRadi;
			}

			currentPt=(currentPt+1)%3;
			if(!currentPt){//Add polygon to model
				for(int a=0;a<3;++a){//Scale model down
					constructionPts[a]/=constructionRadi;
				}
				currentModel.addPoly(PolygonT(constructionPts,currentColor));
				statusBuffer.addString("PolygonT Added",100,VectorT(position.x,position.y+dims.y*.2),.1,VectorT(0,1,1));
			}
		}

	}

	void updateButtons(){
		setButton.update();
		resetButton.update();
		saveButton.update();
		undoButton.update();
		defaultButton.update();

		if(setButton.getActive()&&setButton.getMouseClicked()){//Set hit
			globalModels[MODEL_SHIP_USER].cleanup();
			currentModel.writeModel("tmp");
			globalModels[MODEL_SHIP_USER].readModel("tmp");
			statusBuffer.addString("User Ship Set",100,VectorT(position.x,position.y+dims.y*.2),.1,VectorT(0,1,1));
		}else if(resetButton.getActive()&&resetButton.getMouseClicked()){//Reset hit
			currentModel.reset();
			statusBuffer.addString("User Ship Reset",100,VectorT(position.x,position.y+dims.y*.2),.1,VectorT(0,1,1));
		}else if(saveButton.getActive()&&saveButton.getMouseClicked()){//Save hit
			currentModel.writeModel(userShipModelPath);
			statusBuffer.addString("User Ship Saved",100,VectorT(position.x,position.y+dims.y*.2),.1,VectorT(0,1,1));
		}else if(undoButton.getActive()&&undoButton.getMouseClicked()){//Undo hit
			currentModel.removePoly(currentModel.polyCount-1);
			statusBuffer.addString("Undo Successful",100,VectorT(position.x,position.y+dims.y*.2),.1,VectorT(0,1,1));
		}else if(defaultButton.getActive()&&defaultButton.getMouseClicked()){//Default hit
			currentModel.generateSample("tmp");
			statusBuffer.addString("Default Ship Loaded",100,VectorT(position.x,position.y+dims.y*.25),.1,VectorT(0,1,1));
		}
	}

	virtual void resize(VectorT windowDims,VectorT oldDims){
		Dlg::resize(windowDims,oldDims);
		constructionRadi=dims.x*.33;
		constructionOrigin=position+dims/2;
	
		setButton.resize(windowDims,oldDims);
		resetButton.resize(windowDims,oldDims);
		saveButton.resize(windowDims,oldDims);
		undoButton.resize(windowDims,oldDims);
		defaultButton.resize(windowDims,oldDims);

		colorPicker.resize(windowDims,oldDims);
	}

	void cleanup(){
		currentModel.cleanup();
		statusBuffer.cleanup();
		colorPicker.cleanup();
	}
};

#endif