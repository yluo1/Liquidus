#ifndef _TORQUEMISSILEOBJECT_
#define _TORQUEMISSILEOBJECT_



#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"


class TorqueMissileObject:public MissileObject{
public:
	
	TorqueMissileObject(){
	}

	TorqueMissileObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,type,color,position,initialVelocity,radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){

		this->color/=2.0f;
	}

	virtual void display(){
		
		glColor3f(color.x,color.y,color.z);

		float newRad=radius*2;
		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		for(int a=0;a<3;++a){
		glBegin(GL_TRIANGLES);
		glVertex3f(position.x,position.y,position.z);
		glVertex3f(position.x-newRad,position.y+newRad/2.0f,position.z);
		glVertex3f(position.x-newRad,position.y-newRad/2.0f,position.z);
		glEnd();
		}
		glPopMatrix();

	}

	virtual void update(){
		processAcceleration();

		//Update kinetic physics

		velocityRelative+=(velocity-velocityRelative)*.2f;
		
		totalVelocity=velocity+velocityRelative;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		life-=dt;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);

		if(hitPoints<=0||life<=0){//Out of bounds
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,color*.3+.7,.9f,
						fluidRadius*5,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius*5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	virtual void processAcceleration(){
		VectorT color=velocityRelative.normalize();
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;

		VectorT impulsePosition((position.x-direction.x*dt*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*dt*radius)/defaultDisplay.windowHeight,1.0);
		
		mapPtr->addImpulseToBuffer(impulsePosition,this->color,.7,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
	/*	if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.05,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	*/
	}
	
};
#endif
