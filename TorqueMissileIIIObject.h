#ifndef _TORQUEMISSILEIIIOBJECT_
#define _TORQUEMISSILEIIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "TrackingLaserObject.h"
#include "LaserObject.h"


class TorqueMissileIIIObject:public TorqueMissileObject{

private:
	Object **shipObjPtr;

	VectorT *shipPosition;
	float *shipDirectionTheta;
	int fireMode;
	int maxFireMode;
	int cycle;
	int cycleMax;
	bool alternateFire;
public:
	
	TorqueMissileIIIObject(){
	}

	TorqueMissileIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		int fireMode,int maxFireMode,
		Object **shipObjPtr,VectorT *shipPosition,float *shipDirectionTheta,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:TorqueMissileObject(objID,teamID,type,color,
		position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->shipObjPtr=shipObjPtr;
		this->shipPosition=shipPosition;
		this->shipDirectionTheta=shipDirectionTheta;
		this->fireMode=fireMode;
		this->maxFireMode=maxFireMode;

		directionTheta=*shipDirectionTheta;
		direction=VectorT(	defaultSinValues[thetaToSinTableIndex(90+directionTheta+fireMode*(360.0/maxFireMode),SIN_TABLE_SIZE)],
							defaultSinValues[thetaToSinTableIndex(directionTheta+fireMode*(360.0/maxFireMode),SIN_TABLE_SIZE)],0);
		position=*shipPosition+direction*radius;

		cycle=0;
		cycleMax=5;
		alternateFire=true;
		blowOnContact=false;
	}
	
	~TorqueMissileIIIObject(){

	}
	
	virtual void display(){
	
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta+180+fireMode*(360/maxFireMode),0,0,1);
		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,1.0);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TORQUE_MISSILEIII].texID);

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(1,1); glVertex2f(radius,radius);
		glTexCoord2d(0,1); glVertex2f(-radius,radius);
		glTexCoord2d(1,0); glVertex2f(radius,-radius);
		glTexCoord2d(0,0); glVertex2f(-radius,-radius);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
		glPopAttrib();
	}

	virtual void update(){

		VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
		
		if(*shipObjPtr!=NULL){
			VectorT lastPosition=position;
			directionTheta=*shipDirectionTheta;
			direction=VectorT(	defaultSinValues[thetaToSinTableIndex(90+directionTheta+fireMode*(360/maxFireMode),SIN_TABLE_SIZE)],
								defaultSinValues[thetaToSinTableIndex(directionTheta+fireMode*(360/maxFireMode),SIN_TABLE_SIZE)],0);
			position=*shipPosition+direction*radius*2;
			totalVelocity=(position-lastPosition)/dt;
			
			if(cycle>=cycleMax){
				int a=findNextOpenObject(firstElementPtr,objCount);
				if(a!=-1){
					firstElementPtr[a]=new LaserIIObject(a,teamID,type,color*.35,position,direction,
										 radius*.25,damage,life*.25,damage,
										 3,
										 mapPtr,firstElementPtr,&firstElementPtr[a]);
				}
				cycle=0;
			}
			++cycle;
		}

		if(hitPoints<=0||*shipObjPtr==NULL){//On death or host shipObjPtr dead
			mapPtr->addImpulseToBuffer(impulsePosition,color*.2,.9,
						fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
};
#endif
