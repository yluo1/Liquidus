#ifndef _HOMINGMISSILEIIOBJECT_
#define _HOMINGMISSILEIIOBJECT_

#include "VectorT.h"

#include "Constants.h"
#include "ConstantsIO.h"

#include "ForcePongConstants.h"
#include "HomingMissileObject.h"


class  HomingMissileIIObject:public HomingMissileObject{
private:
	int fireMode;
	int moveCycle;
	int maxCycle;
public:
	
	HomingMissileIIObject(){
	}

	HomingMissileIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		VectorT *target,int fireMode,int maxFireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:HomingMissileObject(objID,teamID,type,color,
		position,initialVelocity,
		radius,mass,damage,life,hitPoints,
		target,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->fireMode=fireMode;
		moveCycle=0;
		maxCycle=200;
		this->color=color*.8+.2;
		
		velocityRelative=velocityRelative.rotateZAxis(360.0f/RADIAN*((float)fireMode/maxFireMode));

		totalVelocity=velocityRelative;

	}

	virtual ~HomingMissileIIObject(){
		
	}

	virtual void display(){

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_HOME_MISSILEII].texID);
		float newRad=radius;

		if(moveCycle<=maxCycle*.66){//Move into sneak mode
			//Change buffer to ink
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

			glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

			glColor4f(color.x,color.y,color.z,1.0);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(0,1); glVertex2f(position.x+newRad,position.y+newRad);
			glTexCoord2d(1,1); glVertex2f(position.x-newRad,position.y+newRad);
			glTexCoord2d(0,0); glVertex2f(position.x+newRad,position.y-newRad);
			glTexCoord2d(1,0); glVertex2f(position.x-newRad,position.y-newRad);
			glEnd();
			glPopAttrib();

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		
		}

		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,1.0);
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
		//Update kinetic physics

		if(moveCycle>maxCycle*.66){
			processAcceleration();
			velocityRelative+=((*target-position).normalize()*MAX_FLUID_SPEED*.25-velocityRelative)*.08;
		}
		totalVelocity=velocity+velocityRelative;

		processBoundary();
		position+=totalVelocity*dt*globalScale;

		if(moveCycle<maxCycle) ++moveCycle;
		else moveCycle=0;

		life-=dt;
		direction=velocityRelative.normalize();
		directionTheta=atan2(direction.y,direction.x);
		
		if(hitPoints<=0||life<=0||position.x>=defaultDisplay.windowWidth-2||position.y>=defaultDisplay.windowHeight-2||position.x<=1||position.y<=1){//Out of bounds
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
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

	virtual void processAcceleration(){
		VectorT color=velocityRelative.normalize();
		color=(color+1.0)/2.0;
		color.y=1.0-color.y;
		
		VectorT impulsePosition((position.x-direction.x*dt*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*dt*radius)/defaultDisplay.windowHeight,1.0);	
		mapPtr->addImpulseToBuffer(impulsePosition,color*.5,.25,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.1,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}
	}
	
};
#endif
