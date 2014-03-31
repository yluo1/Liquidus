#ifndef _HOMINGMISSILEOBJECT_
#define _HOMINGMISSILEOBJECT_

#include "VectorT.h"

#include "Constants.h"
#include "ConstantsIO.h"
#include "ForcePongConstants.h"
#include "MissileObject.h"


class  HomingMissileObject:public MissileObject{
protected:
	VectorT *target;
public:
	
	HomingMissileObject(){
	}

	HomingMissileObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		VectorT *target,Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,type,color,position,initialVelocity,radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		this->target=target;
	}

	virtual ~HomingMissileObject(){
		
	}

	virtual void display(){
		glColor4f(color.x*.25,color.y*.25,color.z*.25,1.0);

		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glBegin(GL_TRIANGLES);
		glVertex3f(position.x,position.y,position.z);
		glVertex3f(position.x-radius,position.y+radius/2.0f,position.z);
		glVertex3f(position.x-radius,position.y-radius/2.0f,position.z);
		glEnd();

		VectorT colorOutline=color*.5+.5;
		glColor4f(colorOutline.x,colorOutline.y,colorOutline.z,1.0);
		glBegin(GL_LINE_LOOP);
		glVertex3f(position.x,position.y,position.z);
		glVertex3f(position.x-radius,position.y+radius/2.0f,position.z);
		glVertex3f(position.x-radius,position.y-radius/2.0f,position.z);
		glEnd();

		glPopMatrix();

	}

	virtual void update(){
		processAcceleration();

		//Update kinetic physics

		velocityRelative+=((*target-position).normalize()*MAX_FLUID_SPEED*.125-velocityRelative)*.08;
		totalVelocity=velocity+velocityRelative;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		life-=dt;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);
		
		if(hitPoints<=0||life<=0){//Dead
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			for(int a=0;a<5;++a){
				mapPtr->addImpulseToBuffer(impulsePosition,(-color+1)*.2+.8f,.9f,
							fluidRadius*7/(a+1),mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
				if(CHAOTIC_MODE){
						mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
								fluidRadius*6/(a+1),mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
				}
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	virtual void processAcceleration(){
		VectorT color=velocityRelative.normalize();
		VectorT origColor=(color*(1-.5)+.5);
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;
		
		VectorT impulsePosition((position.x-direction.x*dt*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*dt*radius)/defaultDisplay.windowHeight,1.0);	
		mapPtr->addImpulseToBuffer(impulsePosition,(-origColor+1),.25,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.1,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	}
	
};
#endif
