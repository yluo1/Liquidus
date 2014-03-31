#ifndef _MISSILEOBJECT_
#define _MISSILEOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "FluidObject.h"


class MissileObject:public FluidObject{
protected:
	float life;
	float initialLife;
	int type;
public:
	float damage;

	MissileObject(){
	}

	MissileObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:FluidObject(objID,teamID,TYPE_OBJ_MISSILE,color,position,initialVelocity,radius,mass,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->damage=damage;
		this->life=initialLife=life;
		this->type=type;

		blowOnContact=true;
	}
	
	virtual ~MissileObject(){
	
	}

	virtual void display(){
	
		glColor4f(color.x,color.y,color.z,1.0);

		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);


		float newRad=radius*2;
		glBegin(GL_TRIANGLES);
		glVertex2f(position.x,position.y);
		glVertex2f(position.x-newRad,position.y+newRad/2.0f);
		glVertex2f(position.x-newRad,position.y-newRad/2.0f);
		glEnd();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		VectorT colorOutline=color*.5;
		glColor4f(colorOutline.x,colorOutline.y,colorOutline.z,1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(position.x,position.y);
		glVertex2f(position.x-newRad,position.y+newRad/2.0f);
		glVertex2f(position.x-newRad,position.y-newRad/2.0f);
		glEnd();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glPopMatrix();

	}
	virtual void update(){
		processAcceleration();

		//Update kinetic physics
		totalVelocity=velocity+velocityRelative;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;
		life-=dt;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);

		if(hitPoints<=0||life<=0){
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,color,.5f,
						fluidRadius*4,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius*4,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}

			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	void processAcceleration(){
		VectorT color=velocityRelative.normalize();
		VectorT origColor=(color*(1-.5)+.5);
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;

		VectorT impulsePosition((position.x-direction.x*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*radius)/defaultDisplay.windowHeight,1.0);
		mapPtr->addImpulseToBuffer(impulsePosition,this->color,.7,fluidRadius*1.0,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);	
		if(initialLife-life>initialLife*.2){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.3,fluidRadius*1.3,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	}

	virtual float getMaxSpeed(){
		if(CHAOTIC_MODE) return MAX_FLUID_SPEED+250;
		else return 250;
	}
	
};
#endif
