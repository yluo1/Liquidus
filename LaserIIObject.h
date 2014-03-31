#ifndef _LASERIIOBJECT_
#define _LASERIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

class LaserIIObject:public LaserObject{
private:
	float movementSpeed;

public:

	LaserIIObject(){
	}

	LaserIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float damage,float life,float hitPoints,
		int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,fireMode,
		mapPtr,firstElementPtr,currentElementPtr){
		
		movementSpeed=5/dt;

		switch(fireMode){
			case 0:
				this->color=color*.2+.8;
				break;
			case 1:
				velocityRelative=velocityRelative.rotateZAxis(20/RADIAN);
				this->color=(-(color*.2+.8)+1)*.2;
			break;
			case 2:
				velocityRelative=velocityRelative.rotateZAxis(-20/RADIAN);
				this->color=(-(color*.2+.8)+1)*.2;
			break;
		};

		this->color=(color+this->color)*.5;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);
	}
	
	virtual ~LaserIIObject(){
		
	}

	virtual void display(){

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glScalef(-3,1,1);
		glTranslatef(-position.x,-position.y,-position.z);


		glEnable(GL_TEXTURE_2D);
		VectorT color=this->color*.75+.25;
		glColor4f(color.x,color.y,color.z,.5);

		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_LASERII].texID);

		float newRad=radius;
		
		glBegin(GL_TRIANGLE_STRIP);
		for(int a=0;a<3;++a){
			glTexCoord2d(1,1); glVertex2f(position.x+newRad,position.y+newRad);
			glTexCoord2d(0,1); glVertex2f(position.x-newRad,position.y+newRad);
			glTexCoord2d(1,0); glVertex2f(position.x+newRad,position.y-newRad);
			glTexCoord2d(0,0); glVertex2f(position.x-newRad,position.y-newRad);
		}
		glEnd();
		

		glPopAttrib();
		glPopMatrix();
	}

	virtual void update(){
		VectorT tmp;
		VectorT lastPosition=position;

		position+=direction*movementSpeed*globalScale*dt;
		totalVelocity=(position-lastPosition)/dt;
		
		VectorT rayColor=color;
		VectorT impulsePosition((position.x)/defaultDisplay.windowWidth,1-(position.y)/defaultDisplay.windowHeight,1.0);
		mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.33,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
	
		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE){
				VectorT color=direction.normalize();
				color=(color+1.0)/2.0;
				color.y=1.0-color.y;
				mapPtr->addImpulseToBuffer(impulsePosition,color,.25,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
			}
		}
		life-=dt;

		if(hitPoints<=0||position.x>=defaultDisplay.windowWidth-2||position.y>=defaultDisplay.windowHeight-2||position.x<=1||position.y<=1){
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,color,.65f,
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


};
#endif
