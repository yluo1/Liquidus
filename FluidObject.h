#ifndef _FLUIDOBJECT_
#define _FLUIDOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"
#include "Model.h"
#include "Display.h"

#include "ForcePongConstants.h"
#include "Map.h"


class FluidObject:public Object{
protected:
	float hitPoints;
	float fluidRadius;

	VectorT velColor;

	bool blowOnContact;

	int lastDmgSource;

	Map *mapPtr;
	Model *model;
public:
	int objType;//Type of object
	int objID;//Unique ID
	int teamID;//Team's unique ID
	
	bool isActive;
	bool fluidAffected;//Affected by fluid

	VectorT direction;
	float directionTheta;
	VectorT totalVelocity;
	VectorT velocityRelative;//object velocity relative to sitting fluid
	
	VectorT color;
	
	float dt;

	////////////////////////////////////

	FluidObject(){
		mapPtr=NULL;
		model=NULL;
	}

	FluidObject(int objID,int teamID,int objType,VectorT color,VectorT position,VectorT initialVelocity,float radius,float mass,float hitPoints,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:Object(position,radius*globalScale,mass,firstElementPtr,currentElementPtr){
		
		this->objID=objID;
		this->teamID=teamID;
		this->objType=objType;
		this->color=color;
		this->mapPtr=mapPtr;	
		this->hitPoints=hitPoints;

		velocityRelative=initialVelocity;
		if(velocityRelative.norm()==0) velocityRelative=VectorT(1,0,0);
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);
		totalVelocity=velocityRelative;

		fluidAffected=true;
		isActive=true;
		blowOnContact=false;
		
		velColor=-totalVelocity.normalize()+1;
		fluidRadius=ceilf(this->radius*((float)(mapPtr->frameBufferWidth)/defaultDisplay.windowWidth));
	
		lastDmgSource=NULL;
		model=NULL;
		dt=deltaTime;

	}
	

	virtual void display(){
		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);
		if(model==NULL){
			glBegin(GL_POINTS);
			glVertex3f(position.x,position.y,position.z);
			glEnd();
		}else{
			model->display();
		}
		glPopMatrix();
	}

	void displayHighlights(){
		if(isActive){
			glEnable(GL_TEXTURE_2D);
			
			float edgeRad=radius*3;
			glColor4f(color.x,color.y,color.z,.85);
			glBindTexture(GL_TEXTURE_2D, lightTextures[LIGHT_CORONA].texID);

			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1.0,1.0);
			glVertex2f(position.x+edgeRad,position.y+edgeRad); 

			glTexCoord2d(1.0,0.0);
			glVertex2f(position.x+edgeRad,position.y-edgeRad); 

			glTexCoord2d(0.0,1.0);
			glVertex2f(position.x-edgeRad,position.y+edgeRad);
			
			glTexCoord2d(0.0,0.0);
			glVertex2f(position.x-edgeRad,position.y-edgeRad);

			glEnd();

			glDisable(GL_TEXTURE_2D);
		}

	}

	void displayTargetLock(VectorT position,float minRadius,VectorT &color){
		//Draw target lock
		glColor4fv((GLfloat*)&color);
		float targetRadius=maxValue(radius,minRadius);

		glBegin(GL_LINE_STRIP);
		glVertex2f(position.x+targetRadius*.66,position.y+targetRadius);
		glVertex2f(position.x+targetRadius,position.y+targetRadius);
		glVertex2f(position.x+targetRadius,position.y+targetRadius*.66);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex2f(position.x+targetRadius*.66,position.y-targetRadius);
		glVertex2f(position.x+targetRadius,position.y-targetRadius);
		glVertex2f(position.x+targetRadius,position.y-targetRadius*.66);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex2f(position.x-targetRadius*.66,position.y+targetRadius);
		glVertex2f(position.x-targetRadius,position.y+targetRadius);
		glVertex2f(position.x-targetRadius,position.y+targetRadius*.66);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex2f(position.x-targetRadius*.66,position.y-targetRadius);
		glVertex2f(position.x-targetRadius,position.y-targetRadius);
		glVertex2f(position.x-targetRadius,position.y-targetRadius*.66);
		glEnd();
	}

	virtual void update(){
		acceleration=(force/mass);
		velocityRelative+=acceleration*(dt);
		totalVelocity=velocity+velocityRelative;
		processBoundary();
		position+=totalVelocity*dt;
		direction=VectorT(	cos(directionTheta/RADIAN),
							sin(directionTheta/RADIAN),0);

		if(directionTheta>=360) directionTheta=0;
		else if(directionTheta<0) directionTheta=360;

		if(hitPoints<=0){
			*currentElementPtr=NULL;
			delete this;
		}
	}

	virtual void accelerate(){
		VectorT color=direction;
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;

		VectorT impulsePosition=VectorT(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,0.0);
		mapPtr->addImpulseToBuffer(	impulsePosition,color,.1,fluidRadius,
									mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
	}
	
	virtual float getMaxSpeed(){
		return MAX_FLUID_SPEED;
	}

	void applyDamage(float dmg,int dmgType,int sourceDmgID){
		if(blowOnContact&&dmgType==TYPE_OBJ_SHIP) hitPoints=0;
		hitPoints-=dmg;	
		lastDmgSource=sourceDmgID;
	}

	void turnLeft(){
		if(isActive){
			directionTheta-=4;
			if(directionTheta<0) directionTheta+=360;
		}
	}
	void turnRight(){
		if(isActive){
			directionTheta+=4;
			if(directionTheta>=360) directionTheta-=360;
		}
	}


	void processBoundary(){
		VectorT nextPosition=position+totalVelocity*dt*globalScale;
		if(nextPosition.x<radius||nextPosition.x>defaultDisplay.windowWidth-1-radius){
			//position.x-=totalVelocity.x*dt*globalScale+.001;
			position.x=minValue(maxValue((position.x-totalVelocity.x*dt*globalScale+.001),radius),defaultDisplay.windowWidth-1-radius);

			velocity.x*=-1;
			velocityRelative.x*=-1;
			force.x*=-1;
			direction=velocityRelative.normalize();
			directionTheta=atan2(direction.y,direction.x);
		}
		if(nextPosition.y<radius||nextPosition.y>defaultDisplay.windowHeight-1-radius){
			//position.y-=totalVelocity.y*dt*globalScale+.001;
			position.y=minValue(maxValue((position.y-totalVelocity.y*dt*globalScale+.001),radius),defaultDisplay.windowHeight-1-radius);

			velocity.y*=-1;
			velocityRelative.y*=-1;
			force.y*=-1;
			direction=velocityRelative.normalize();
			directionTheta=atan2(direction.y,direction.x);
		}
	}


	void addHp(float value){
		hitPoints+=value;
	}

	int getLastDmgSource(){
		return lastDmgSource;
	}
	float getHp(){
		return hitPoints;
	}

	void setHp(float value){
		hitPoints=value;
	}

};
#endif
