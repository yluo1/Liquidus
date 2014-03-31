#ifndef _LASERIIIOBJECT_
#define _LASERIIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

class LaserIIIObject:public LaserObject{
private:
	float maxSpeed;

	Object **shipObjPtr;
	float *shipPower;
	VectorT *shipPosition;
	
	float powerDrain;
	bool charging;
	int charge;
	float baseRadius;
public:

	LaserIIIObject(){
	}

	LaserIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,float *shipPower,VectorT *shipPosition,float shipPowerRegen,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,0,
		mapPtr,firstElementPtr,currentElementPtr){

		this->shipObjPtr=shipObjPtr;
		this->shipPower=shipPower;
		this->shipPosition=shipPosition;

		powerDrain=shipPowerRegen*(1.15+(rand()%100)/100.0f*.15f);
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);

		velocityRelative=initialVelocity;
		totalVelocity=velocityRelative;
		maxSpeed=MAX_FLUID_SPEED+velocityRelative.norm();

		charge=0;
		baseRadius=radius;
		blowOnContact=false;
		charging=true;
	}
	
	virtual ~LaserIIIObject(){
		
	}

	virtual void display(){

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		VectorT color=this->color*.75+.25;
		glColor4f(color.x,color.y,color.z,1);

		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_LASERIII].texID);

		float newR=radius*2;
		glBegin(GL_TRIANGLE_STRIP);
		for(int a=0;a<2;++a){
			glTexCoord2d(1,1); glVertex2f(position.x+newR,position.y+newR);
			glTexCoord2d(0,1); glVertex2f(position.x-newR,position.y+newR);
			glTexCoord2d(1,0); glVertex2f(position.x+newR,position.y-newR);
			glTexCoord2d(0,0); glVertex2f(position.x-newR,position.y-newR);
		}
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}

	virtual void update(){
		if(*shipPower<=.1||*shipObjPtr==NULL||radius>baseRadius*10){//stop charging and fire when power is low or host ship dies
			charging=false;
		}

		if(directionTheta>=360) directionTheta=0;
		directionTheta+=5.0;

		if(charging){
			radius+=.12;
			hitPoints+=10;
			damage+=.001;
			fluidRadius=ceilf(radius*((float)(mapPtr->frameBufferWidth)/defaultDisplay.windowWidth));
			*shipPower-=powerDrain;
			position=*shipPosition;
			++charge;
		}else{
			VectorT lastPosition=position;
			totalVelocity=velocity+velocityRelative;
			processBoundary();
			position+=totalVelocity*dt*globalScale;

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	


			VectorT rayColor;
			switch(rand()%3){
				case 0: rayColor=(VectorT(1,0,0)+color)/2; break;
				case 1: rayColor=(VectorT(0,1,0)+color)/2; break;
				case 2: rayColor=(VectorT(0,0,1)+color)/2; break;
			};
			VectorT ipr((position.x+rand()%maxValue((int)radius,1)-radius/2)/defaultDisplay.windowWidth,
									1-(position.y+rand()%maxValue((int)radius,1)-radius/2)/defaultDisplay.windowHeight,1.0);
			mapPtr->addImpulseToBuffer(ipr,rayColor,.33,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);


			if(initialLife-life>initialLife*.1){
				if(CHAOTIC_MODE){
					VectorT color=direction.normalize();
					color=(color+1.0)/2.0;
					color.y=1.0-color.y;
					mapPtr->addImpulseToBuffer(impulsePosition,color,.05,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
				}
			}
			life-=dt;
			if(life<=0){//explode
				for(int a=0;a<charge/20;++a){
					VectorT ip((position.x+rand()%((int)radius*6)-radius*3)/defaultDisplay.windowWidth,
							1-(position.y+rand()%((int)radius*6)-radius*3)/defaultDisplay.windowHeight,1.0);
					mapPtr->addImpulseToBuffer(ip,VectorT(1,1,1),1.0,
									fluidRadius*3,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
				}
				if(CHAOTIC_MODE){
					mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
							fluidRadius*5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
				}
				(*currentElementPtr)=NULL;
				delete this;
			}
		}
	}
	
	virtual float getMaxSpeed(){
		return maxSpeed;
	}

};
#endif
