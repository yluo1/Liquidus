#ifndef _CHAOSMISSILEIIOBJECT_
#define _CHAOSMISSILEIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"

class ChaosMissileIIObject:public MissileObject{
private:
	int spreadCycle;
	int maxSpreadCycle;
	int depth;
	int maxSpread;
public:
	
	ChaosMissileIIObject(){
	}

	ChaosMissileIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		int maxSpreadCycle,int depth,int maxSpread,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,type,color,position,initialVelocity,radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){

		this->maxSpreadCycle=maxSpreadCycle;
		this->depth=depth;
		this->maxSpread=maxSpread;
		spreadCycle=0;

		blowOnContact=false;
		fluidAffected=false;
	}

	virtual ~ChaosMissileIIObject(){
		
	}

	virtual void display(){
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

		glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT|GL_ENABLE_BIT);
		glDisable(GL_BLEND);
		glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

		glColor4f(color.x*.5,color.y*.5,color.z*.5,0);

		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_CHAOSII].texID);

		float newRad=radius;
		glBegin(GL_QUADS);
		glTexCoord2d(1,1); glVertex2f(position.x+newRad,position.y+newRad);
		glTexCoord2d(1,0); glVertex2f(position.x+newRad,position.y-newRad);
		glTexCoord2d(0,0); glVertex2f(position.x-newRad,position.y-newRad);
		glTexCoord2d(0,1); glVertex2f(position.x-newRad,position.y+newRad);
		glEnd();

		glPopAttrib();


		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glPopAttrib();


		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_CHAOSII].texID);
		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,(life/initialLife)*.5+.5);
		glBegin(GL_QUADS);
		glTexCoord2d(1,1); glVertex2f(position.x+newRad,position.y+newRad);
		glTexCoord2d(1,0); glVertex2f(position.x+newRad,position.y-newRad);
		glTexCoord2d(0,0); glVertex2f(position.x-newRad,position.y-newRad);
		glTexCoord2d(0,1); glVertex2f(position.x-newRad,position.y+newRad);
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}

	virtual void update(){
		if(depth<2) processAcceleration();

		//Update kinetic physics
		totalVelocity=velocity+velocityRelative;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		life-=dt;
		directionTheta=1000.0/(rand()%1000)*2*PI;

		if(spreadCycle<maxSpreadCycle) ++spreadCycle;
		else{//Propagate
			if(depth<maxSpread){
				for(int b=0;b<1;++b){
					int a=findNextOpenObject(firstElementPtr,objCount);
					if(a!=-1){
						firstElementPtr[a]=new ChaosMissileIIObject(a,teamID,type,color*.75+.25,position,
										VectorT((rand()%1000/1000.0)-.5,(rand()%1000/1000.0)-.5,0)*2*250,
										radius*1.33,mass*.5,damage*.333,initialLife*.53,hitPoints*.33,
										maxSpreadCycle,depth+1,maxSpread,
										mapPtr,firstElementPtr,&firstElementPtr[a]);
					}
				}
			}

			spreadCycle=0;
		}

		if(hitPoints<=0||life<=0){//Life end so spawn bits
			if(depth<4){
				VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
				if(CHAOTIC_MODE){
					mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
							fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
				}
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}

	virtual void processAcceleration(){
		VectorT color=velocityRelative.normalize();
		VectorT origColor=(color*(1-.5)+.5);

		color=(color+1.0)/2.0;
		color.y=1.0-color.y;
				
		VectorT impulsePosition((position.x-direction.x*dt*radius)/defaultDisplay.windowWidth,1-(position.y-direction.y*dt*radius)/defaultDisplay.windowHeight,1.0);
		
		mapPtr->addImpulseToBuffer(impulsePosition,this->color*.25,.7,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			
		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.2,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}

	}
	
};
#endif
