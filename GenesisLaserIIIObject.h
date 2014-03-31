#ifndef _GENESISLASERIIIOBJECT_
#define _GENESISLASERIIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

#define GENESISLASERIIIPOINTS 10

class GenesisLaserIIIObject:public LaserObject{
private:
	float maxLength;
	VectorT *sourcePosition;
	float *sourceDirectionTheta;
	int fireMode;
	VectorT lastPosition;

	Object **shipObjPtr;
public:
	
	GenesisLaserIIIObject(){
	}

	GenesisLaserIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *sourcePosition,float *sourceDirectionTheta,int fireMode,float maxLength,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,
		position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){

		this->sourcePosition=sourcePosition;
		this->sourceDirectionTheta=sourceDirectionTheta;
		this->maxLength=maxLength;
		this->fireMode=fireMode;
		this->shipObjPtr=shipObjPtr;

		blowOnContact=false;
		fluidAffected=false;
		
		if(fireMode>0){
			this->position=*sourcePosition;
			switch(fireMode){
				case 1:
				this->position.x+=maxLength;
				break;
				case 2:
				this->position.x+=maxLength/1.5;
				break;
				case 3:
				this->position.x+=maxLength/2.0;
				break;
				case 4:
				this->position.x+=maxLength/2.5;
				break;
			};
			mass=.0001/fireMode;
		}
		directionTheta=*sourceDirectionTheta;

	}
	
	virtual void display(){

		if(fireMode==0){//Base Shot
			/*
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
			glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

			displayBlades(this->color*.25);
			
			glPopAttrib();

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			*/

			displayBlades(VectorT(1,1,1));

		}
	}
	
	void displayBlades(VectorT color){
		float newRad=radius;

		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_GENESIS_LASERIII].texID);
			
		for(int a=0;a<4;++a){
			if((a&0x00000001)==0) glColor4f(color.x*.5,color.y*.5,color.z*.5,.5+life/initialLife*.5);
			else 				  glColor4f(color.x*.2,color.y*.2,color.z*.2,.5+life/initialLife*.5);
			glPushMatrix();
			glTranslatef(position.x,position.y,position.z);
			glRotatef(directionTheta+a*90,0,0,1);
			glTranslatef(-position.x,-position.y,-position.z);


			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1,1); glVertex2f(position.x,position.y+newRad);
			glTexCoord2d(0,1); glVertex2f(position.x+maxLength/(a/2.0+1),position.y+newRad);
			glTexCoord2d(1,0); glVertex2f(position.x,position.y-newRad);
			glTexCoord2d(0,0); glVertex2f(position.x+maxLength/(a/2.0+1),position.y-newRad);
			glEnd();

			glPopMatrix();
		}
		glPopAttrib();
	}

	virtual void update(){
		if(fireMode==0){
			if(life==initialLife){
				for(int spoke=0;spoke<4;++spoke){
					int nBits=maxLength/(spoke/2.0+1)/radius;
					for(int a=0;a<nBits;++a){
						int b=findNextOpenObject(firstElementPtr,objCount);
						if(b!=-1){
							firstElementPtr[b]=new GenesisLaserIIIObject(b,teamID,type,color,position,velocityRelative,
														radius,damage,life,hitPoints,
														shipObjPtr,sourcePosition,sourceDirectionTheta,spoke+1,maxLength/nBits*a,
														mapPtr,firstElementPtr,&firstElementPtr[b]);
						}
					}
				}
			}

		}
		
		if(*shipObjPtr!=NULL){
			directionTheta=*sourceDirectionTheta;
			lastPosition=position;
			position=*sourcePosition;

			switch(fireMode){
				case 1:
				position.x+=maxLength;
				break;
				case 2:
				position.x+=maxLength/1.5;
				break;
				case 3:
				position.x+=maxLength/2.0;
				break;
				case 4:
				position.x+=maxLength/2.5;
				break;
			};

			position=position.translate(-*sourcePosition);
			position=position.rotateZAxis((directionTheta+(fireMode-1)*90)/RADIAN*-1);
			position=position.translate(*sourcePosition);
			
			totalVelocity=(position-lastPosition)/dt;
	
			
			VectorT rayColor;
			if(fireMode%2==0) rayColor=color*.4+.6;
			else rayColor=color*.2;

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,.5);
			mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.1,
				fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

			if(CHAOTIC_MODE){
				float d=totalVelocity.norm();
				if(d!=0){
					velColor=(totalVelocity/d+1.0)/2.0;
					velColor.y=1.0-velColor.y;
					mapPtr->addImpulseToBuffer(impulsePosition,velColor,.1,
											fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
				}
			}
			
		}

		life-=dt;
		if(life<=0||*shipObjPtr==NULL){//Life end or host ship dead
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	virtual float getMaxSpeed(){
		return MAX_FLUID_SPEED*10;
	}
	
};
#endif
