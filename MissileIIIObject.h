#ifndef _MISSILEIIIOBJECT_
#define _MISSILEIIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"


class MissileIIIObject:public MissileObject{
private:
	VectorT *targetVectorT;
	int maxBounce;
	int fireMode;

	int bounce;
	float velocityMagnitude;

public:

	MissileIIIObject(){
	}

	MissileIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		VectorT *targetVectorT,int maxFireMode,int fireMode,int maxBounce,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,TYPE_OBJ_MISSILE,color,position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->targetVectorT=targetVectorT;
		this->maxBounce=maxBounce;
		this->fireMode=fireMode;
		bounce=0;

		velocityMagnitude=velocityRelative.norm();
		velocityRelative=velocityRelative.rotateZAxis(fireMode*(360.0/maxFireMode)/RADIAN);
	
	}
	
	virtual ~MissileIIIObject(){
	
	}

	virtual void display(){
	
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		
		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z,.5*.25+.75);
		
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_MISSILEIII].texID);

		float newRad=radius*2;
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(0,1); glVertex2f(position.x+newRad,position.y+newRad);
		glTexCoord2d(1,1); glVertex2f(position.x-newRad,position.y+newRad);
		glTexCoord2d(0,0); glVertex2f(position.x+newRad,position.y-newRad);
		glTexCoord2d(1,0); glVertex2f(position.x-newRad,position.y-newRad);
		glEnd();


		glPopAttrib();
		glPopMatrix();

	}
	virtual void update(){
		processAcceleration();

		//Update kinetic physics
		totalVelocity=velocity*.5+velocityRelative.normalize()*velocityMagnitude;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);
		
		life-=dt;
		if(hitPoints<=0||bounce>=maxBounce){
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
		mapPtr->addImpulseToBuffer(impulsePosition,(color+this->color)*.5,.7,fluidRadius*1.0,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);	

		if(initialLife-life>initialLife*.2){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.7,fluidRadius*1.5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}

	}

	void processBoundary(){
		VectorT nextPosition=position+totalVelocity*dt;
		if(nextPosition.x<radius||nextPosition.x>defaultDisplay.windowWidth-1-radius){
			position.x-=totalVelocity.x*dt+.001;

			velocity*=-1;
			velocityRelative=(*targetVectorT-position).normalize()*velocityMagnitude;
			++bounce;
		}
		if(nextPosition.y<radius||nextPosition.y>defaultDisplay.windowHeight-1-radius){
			position.y-=totalVelocity.y*dt+.001;

			velocity.y*=-1;
			velocityRelative=(*targetVectorT-position).normalize()*velocityMagnitude;
			++bounce;
		}
	}


	float getMaxSpeed(){
		if(CHAOTIC_MODE) return (MAX_FLUID_SPEED*.5+velocityMagnitude);
		else return 250;
	}
	
};
#endif
