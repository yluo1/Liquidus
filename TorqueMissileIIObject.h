#ifndef _TORQUEMISSILEIIOBJECT_
#define _TORQUEMISSILEIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "TorqueMissileObject.h"


class TorqueMissileIIObject:public TorqueMissileObject{
private:
	int fireMode;
	int maxFireMode;
	float spinRadius;
	VectorT rotVelocity;
	TorqueMissileIIObject *last;
	TorqueMissileIIObject *next;

public:
	
	TorqueMissileIIObject(){
	}

	TorqueMissileIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		int fireMode,int maxFireMode,
		TorqueMissileIIObject *last,TorqueMissileIIObject *next, 
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:TorqueMissileObject(objID,teamID,type,color,
		position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){
	
		this->fireMode=fireMode;
		this->maxFireMode=maxFireMode;
		this->next=next;
		this->last=last;
		spinRadius=16;

		rotVelocity=velocityRelative;
		rotVelocity=rotVelocity.rotateZAxis(fireMode*(360.0/maxFireMode)/RADIAN);
		fluidAffected=false;
	}

	void display(){

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		
		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,1.0);
		
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TORQUE_MISSILEII].texID);

		float newRad=radius*2;
		glBegin(GL_TRIANGLE_STRIP);
		for(int a=0;a<2;++a){
			glTexCoord2d(0,1); glVertex2f(position.x+newRad,position.y+newRad);
			glTexCoord2d(1,1); glVertex2f(position.x-newRad,position.y+newRad);
			glTexCoord2d(0,0); glVertex2f(position.x+newRad,position.y-newRad);
			glTexCoord2d(1,0); glVertex2f(position.x-newRad,position.y-newRad);
		}
		glEnd();
	

		glPopAttrib();
		glPopMatrix();

		if(next!=NULL){
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

			glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

			glColor4f(color.x,color.y,color.z,.5);
			glBegin(GL_LINES);
			glVertex3fv((GLfloat*)&position);
			glVertex3fv((GLfloat*)&(next->position));
			glEnd();

			glPopAttrib();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			
		}

	}

	void update(){
		processAcceleration();

		//Update kinetic physics
		
		rotVelocity=rotVelocity.rotateZAxis(spinRadius/RADIAN);
		spinRadius*=.995;
		
		totalVelocity=velocityRelative+rotVelocity;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		life-=dt;
		direction=velocityRelative.normalize();
		VectorT tmpDirection=(rotVelocity).normalize();
		directionTheta=atan2(tmpDirection.y,tmpDirection.x);

		if(hitPoints<=0||life<=0){//Dead
			//Update next/last ptrs
			last->next=next;
			next->last=last;

			VectorT origColor=(totalVelocity.normalize()*.5+.5);
			color=(color+1.0)/2.0;
			color.y=1.0-color.y;

			float t=origColor.x;
			origColor.x=origColor.y;
			origColor.y=origColor.z;
			origColor.z=t;

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,origColor,.9f,
						fluidRadius*8,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius*8,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	void processAcceleration(){
		VectorT color=totalVelocity.normalize();
		VectorT origColor=(color*.5+.5);
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;

		float t=origColor.x;
		origColor.x=origColor.y;
		origColor.y=origColor.z;
		origColor.z=t;

		VectorT impulsePosition((position.x-direction.x*dt*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*dt*radius)/defaultDisplay.windowHeight,1.0);
		
		mapPtr->addImpulseToBuffer(impulsePosition,origColor,.7,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.5,fluidRadius*2,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	}

	void setNext(TorqueMissileIIObject *next){
		this->next=next;
	}
	
	void setLast(TorqueMissileIIObject *last){
		this->last=last;
	}

	
};
#endif
