#ifndef _MISSILEIIOBJECT_
#define _MISSILEIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"


class MissileIIObject:public MissileObject{
private:
	int fireMode;

public:

	MissileIIObject(){
	}

	MissileIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,TYPE_OBJ_MISSILE,color,position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->fireMode=fireMode;
		switch(fireMode){
			case 1:
			velocityRelative=velocityRelative.rotateArbitraryAxis(VectorT(0,0,1),30/RADIAN);
			break;
			case 2:
			velocityRelative=velocityRelative.rotateArbitraryAxis(VectorT(0,0,1),-30/RADIAN);
			break;
		}
		if(fireMode!=0){
			this->color=-color+1;
			this->color*=.5;
		}

		this->color=(color+this->color)*.5;
	}
	
	virtual ~MissileIIObject(){
	
	}

	virtual void display(){
	
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		
		glColor4f(color.x*.5+.5,color.y*.5+.5,color.z*.5+.5,1.0);
		
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_MISSILEII].texID);

		float newRad=radius*1.5;
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
		totalVelocity=velocity+velocityRelative;
		if(fireMode==0) processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;
		life-=dt;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);

		if(hitPoints<=0||life<=0||position.x<radius||position.x>defaultDisplay.windowWidth-1-radius||position.y<radius||position.y>defaultDisplay.windowHeight-1-radius){
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

	float getMaxSpeed(){
		if(CHAOTIC_MODE) return (MAX_FLUID_SPEED+250);
		else return 250;
	}
	
};
#endif
