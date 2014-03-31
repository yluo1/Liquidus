#ifndef _HOMINGMISSILEIIIOBJECT_
#define _HOMINGMISSILEIIIOBJECT_

#include "VectorT.h"

#include "Constants.h"
#include "ConstantsIO.h"

#include "ForcePongConstants.h"
#include "HomingMissileObject.h"


class  HomingMissileIIIObject:public HomingMissileObject{
private:
	int fireCycle;
	int maxCycle;
public:
	
	HomingMissileIIIObject(){
	}

	HomingMissileIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		VectorT *target,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:HomingMissileObject(objID,teamID,type,color,
		position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		target,
		mapPtr,firstElementPtr,currentElementPtr){
		
		fireCycle=0;
		maxCycle=200;
		this->color=color*.5+.5;

		totalVelocity=velocityRelative;

	}

	virtual ~HomingMissileIIIObject(){
		
	}

	virtual void display(){

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_HOME_MISSILEIII].texID);
		float newRad=radius;

		glColor4f(color.x,color.y,color.z,1.0);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(0,1); glVertex2f(position.x+newRad,position.y+newRad);
		glTexCoord2d(1,1); glVertex2f(position.x-newRad,position.y+newRad);
		glTexCoord2d(0,0); glVertex2f(position.x+newRad,position.y-newRad);
		glTexCoord2d(1,0); glVertex2f(position.x-newRad,position.y-newRad);
		glEnd();

		glPopAttrib();

		displayHighlights();
		glPopMatrix();

	}

	virtual void update(){
		//Update kinetic physics
		

		totalVelocity=velocity;
		processBoundary();
		position+=totalVelocity*dt*globalScale;

		if(fireCycle<maxCycle) ++fireCycle;
		else{
			fireCycle=0;
			int a=findNextOpenObject(firstElementPtr,objCount);
			if(a!=-1){
				firstElementPtr[a]=new HomingMissileObject(a,teamID,type,(-color+1),
									 position,direction*250,
									 radius*.5,1.0,damage*.25,life*.25,damage*.33,
									 target,
									 mapPtr,firstElementPtr,&firstElementPtr[a]);
			}
		}

		life-=dt;
		direction+=((*target-position).normalize()-direction)*.1;
		directionTheta=atan2(direction.y,direction.x);

		VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
		mapPtr->addImpulseToBuffer(impulsePosition,color*.2,.9f,
							fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		
		if(hitPoints<=0||life<=0){//Out of bounds
			
			for(int a=0;a<5;++a){
				mapPtr->addImpulseToBuffer(impulsePosition,color*.2,.9f,
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
	
};
#endif
