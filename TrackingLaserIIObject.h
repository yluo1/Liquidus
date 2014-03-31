#ifndef _TRACKINGLASERIIOBJECT_
#define _TRACKINGLASERIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "TrackingLaserObject.h"

#define TRACKIIPOINTS 100

class TrackingLaserIIObject:public LaserObject{

private:
	Object **shipObjPtr;
	VectorT *shipPosition;
	float *shipDirectionTheta;
	VectorT *targetVectorT;

	float maxSpeed;
	int fireMode;
	VectorT pts[TRACKIIPOINTS];
	float rotation;
public:
	
	TrackingLaserIIObject(){
	}

	TrackingLaserIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *shipPosition,float *shipDirectionTheta,VectorT *targetVectorT,int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->shipObjPtr=shipObjPtr;
		this->shipPosition=shipPosition;
		this->shipDirectionTheta=shipDirectionTheta;
		this->targetVectorT=targetVectorT;
		this->fireMode=fireMode;
		this->mass=mass;	

		rotation=rand()%360;
		
		for(int a=0;a<TRACKIIPOINTS;++a) pts[a]=VectorT(cos((float)a/TRACKIIPOINTS*2*PI),sin((float)a/TRACKIIPOINTS*2*PI),0)*radius;

		maxSpeed=MAX_FLUID_SPEED/2;
		directionTheta=*shipDirectionTheta;
		direction=VectorT(	defaultSinValues[thetaToSinTableIndex(90+directionTheta+fireMode*120,SIN_TABLE_SIZE)],
							defaultSinValues[thetaToSinTableIndex(directionTheta+fireMode*120,SIN_TABLE_SIZE)],0);
		position=*shipPosition+direction*radius;
	}
	
	~TrackingLaserIIObject(){

	}
	
	virtual void display(){
	
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(rotation,0,0,1);

		 glColor4f(1-color.x,1-color.y,1-color.z,.1+(initialLife-life)/initialLife*.9);
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TRACKING_LASERII].texID);

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(1,1); glVertex2f(radius,radius);
		glTexCoord2d(0,1); glVertex2f(-radius,radius);
		glTexCoord2d(1,0); glVertex2f(radius,-radius);
		glTexCoord2d(0,0); glVertex2f(-radius,-radius);
		glEnd();

		glBegin(GL_LINE_LOOP);
		for(int a=0;a<TRACKIIPOINTS;++a) glVertex3fv((float*)&pts[a]);
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}
	virtual void update(){

		VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);

		if(*shipObjPtr!=NULL){
			if(initialLife-life<initialLife*.5){

				directionTheta=*shipDirectionTheta;
				direction=VectorT(	defaultSinValues[thetaToSinTableIndex(90+directionTheta+fireMode*120,SIN_TABLE_SIZE)],
									defaultSinValues[thetaToSinTableIndex(directionTheta+fireMode*120,SIN_TABLE_SIZE)],0);
				position=*shipPosition+direction*radius;

				velocityRelative=(*targetVectorT-position).normalize()*maxSpeed;
				totalVelocity=velocityRelative;
			}else{
				position+=totalVelocity*dt*globalScale;
				rotation+=20;
				mapPtr->addImpulseToBuffer(impulsePosition,color,.005,
				fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

			
			}
		}

		life-=dt;
		if(life<=0||hitPoints<=0||*shipObjPtr==NULL){//On death
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

	float getMaxSpeed(){
		return maxSpeed;
	}
};
#endif
