#ifndef _TRACKINGLASERIIIOBJECT_
#define _TRACKINGLASERIIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "TrackingLaserObject.h"

class TrackingLaserIIIObject:public LaserObject{

private:
	float maxSpeed;
	float turningSpeed;

	Object **shipObjPtr;

	VectorT targetVectorT;
	VectorT *shipPosition;
	bool isTracking;
	float rotationTheta;
	float deltaRotationTheta;

public:
	
	TrackingLaserIIIObject(){
	}

	TrackingLaserIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		Object** shipObjPtr, VectorT *shipPosition,VectorT targetVectorT,float shipDirectionTheta,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->shipObjPtr=shipObjPtr;
		this->targetVectorT=targetVectorT;
		this->mass=mass;	
		this->shipPosition=shipPosition;

		maxSpeed=MAX_FLUID_SPEED;
		turningSpeed=(rand()%1000)/1000.0*.01+.01;
		rotationTheta=0;
		isTracking=true;

		directionTheta=shipDirectionTheta+(rand()%360)-180;
		deltaRotationTheta=(rand()%1000)/1000.0f*10-5;

		direction=VectorT(	defaultSinValues[thetaToSinTableIndex(90+directionTheta,SIN_TABLE_SIZE)],
							defaultSinValues[thetaToSinTableIndex(directionTheta,SIN_TABLE_SIZE)],0);
		velocityRelative=direction*maxSpeed;

	}
	
	~TrackingLaserIIIObject(){

	}
	
	virtual void display(){

		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,life/initialLife*.9f+.1f);

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(rotationTheta,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TRACKING_LASERIII].texID);

		float newRad=radius*1.5;
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(1,1); glVertex2f(position.x+newRad,position.y+newRad);
		glTexCoord2d(0,1); glVertex2f(position.x-newRad,position.y+newRad);
		glTexCoord2d(1,0); glVertex2f(position.x+newRad,position.y-newRad);
		glTexCoord2d(0,0); glVertex2f(position.x-newRad,position.y-newRad);
		glEnd();

		glPopAttrib();
		glPopMatrix();

	}
	virtual void update(){

		if(life>=initialLife*.75)		isTracking=true;
		else if(life>=initialLife*.5)	isTracking=false;
		else if(life>=initialLife*.25)	isTracking=true;
		else if(life>=0)		isTracking=false;

		if(*shipObjPtr==NULL) isTracking=true;//Keep on tracking last point if host ship dead

		if(isTracking)
			direction+=((targetVectorT-position).normalize()-direction)*turningSpeed;
		else
			direction+=((*shipPosition-position).normalize()-direction)*turningSpeed;
		
		rotationTheta+=deltaRotationTheta;

		velocityRelative=direction*maxSpeed;

		totalVelocity=velocityRelative;
		position+=totalVelocity*dt*globalScale;

		VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
		mapPtr->addImpulseToBuffer(impulsePosition,color*.5,.2,
		fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);


		life-=dt;
		if(life<=0||hitPoints<=0){//On death
			if(hitPoints<=0){
				mapPtr->addImpulseToBuffer(impulsePosition,color*.2,.9,
							fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
				if(CHAOTIC_MODE){
					mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
							fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
				}
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	float getMaxSpeed(){
		return maxSpeed;
	}
};
#endif
