#ifndef _LASEROBJECT_
#define _LASEROBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "FluidObject.h"

class LaserObject:public FluidObject{
private:
	int movementIndex;
	int movementIncrement;
	float movementSpeed;
	VectorT initialPos;
	VectorT rayColor;
protected:
	float life;
	float initialLife;
	int type;//Weapon name ID
	int fireMode;
	
public:
	float damage;

	LaserObject(){
	}

	LaserObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float damage,float life,float hitPoints,
		int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:FluidObject(objID,teamID,TYPE_OBJ_LASER,color,position,initialVelocity,radius,0,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->velocityRelative=initialVelocity;
		this->damage=damage;
		this->type=type;
		this->life=initialLife=life;
		this->fireMode=fireMode;

		initialPos=position;
		movementIndex=0;
		movementIncrement=5/dt;
		movementSpeed=2.5;

		blowOnContact=true;
		fluidAffected=false;
		rayColor=color*.2+.8;

		if(fireMode) rayColor=(-rayColor+1)*.2;
		
		rayColor=(rayColor+color)*.5;
	}
	
	virtual ~LaserObject(){
	
	}

	virtual void display(){

	}
	virtual void update(){
		VectorT tmp;
		VectorT lastPosition=position;
		switch(fireMode){
			case 0:
			position.x=initialPos.x+(movementIndex)*movementSpeed*globalScale;
			position.y=initialPos.y+defaultSinValues[movementIndex%SIN_TABLE_SIZE]*25*globalScale;
			position=position.translate(-initialPos);
			position=position.rotateZAxis(directionTheta*-1);
			position=position.translate(initialPos);
			break;
			case 1:
			position.x=initialPos.x+(movementIndex)*movementSpeed*globalScale;
			position.y=initialPos.y+defaultSinValues[(movementIndex+SIN_TABLE_SIZE/2)%SIN_TABLE_SIZE]*25*globalScale;
			position=position.translate(-initialPos);
			position=position.rotateZAxis(directionTheta*-1);
			position=position.translate(initialPos);
			break;
		};
		totalVelocity=(position-lastPosition)/dt;

		VectorT impulsePosition((position.x)/defaultDisplay.windowWidth,1-(position.y)/defaultDisplay.windowHeight,1.0);
		mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.33,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		if(movementIndex>50){
			if(CHAOTIC_MODE){
				VectorT color=direction;
				VectorT origColor=(color*(1-.5)+.5);
				color=(color+1.0)/2.0;
				color.y=1.0-color.y;
		
				mapPtr->addImpulseToBuffer(impulsePosition,color,.25,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
			}
		}

		movementIndex+=movementIncrement*dt;
		if(hitPoints<=0||position.x>=defaultDisplay.windowWidth-2||position.y>=defaultDisplay.windowHeight-2||position.x<=1||position.y<=1){
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.65f,
						fluidRadius*2,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			/*
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}
			*/
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	
	int getType(){
		return type;
	}
	virtual float getMaxSpeed(){
		return movementIncrement*movementSpeed*dt;
	}

};
#endif
