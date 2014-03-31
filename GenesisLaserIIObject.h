#ifndef _GENESISLASERIIOBJECT_
#define _GENESISLASERIIOBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

#define GENESISLASERIIPOINTS 10

class GenesisLaserIIObject:public LaserObject{
private:
	float maxLength;
	VectorT *sourcePosition;
	float *sourceDirectionTheta;
	int fireMode;
	VectorT lastPosition;

	Object **shipObjPtr;
public:
	
	GenesisLaserIIObject(){
	}

	GenesisLaserIIObject(int objID,int teamID,int type,VectorT color,
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
		
		if(fireMode!=0){
			position=*sourcePosition;
			position.x+=maxLength;
			mass=.0001;
		}
		directionTheta=*sourceDirectionTheta;

	}
	
	virtual void display(){

		if(fireMode==0){//Base Shot
			glPushMatrix();
			glPushAttrib(GL_ENABLE_BIT);

			glTranslatef(position.x,position.y,position.z);
			glRotatef(directionTheta,0,0,1);
			glTranslatef(-position.x,-position.y,-position.z);

			glEnable(GL_TEXTURE_2D);
			VectorT color=this->color*25+.75;
			glColor4f(color.x,color.y,color.z,.1+life/initialLife*.9);

			glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_GENESIS_LASERII].texID);

			float newRad=radius*2;
			glBegin(GL_TRIANGLE_STRIP);
			for(int a=0;a<1;++a){
				glTexCoord2d(1,1); glVertex2f(position.x,position.y+newRad);
				glTexCoord2d(0,1); glVertex2f(position.x+maxLength,position.y+newRad);
				glTexCoord2d(1,0); glVertex2f(position.x,position.y-newRad);
				glTexCoord2d(0,0); glVertex2f(position.x+maxLength,position.y-newRad);
			}
			glEnd();

			glPopAttrib();
			glPopMatrix();


		}


	}
	virtual void update(){
	

		if(fireMode==0){
			if(life==initialLife){
				int nBits=maxLength/radius;
				for(int a=0;a<nBits;++a){
					int b=findNextOpenObject(firstElementPtr,objCount);
					if(b!=-1){
					firstElementPtr[b]=new GenesisLaserIIObject(b,teamID,type,color,position,velocityRelative,
												radius,damage*0,life,hitPoints,
												shipObjPtr,sourcePosition,sourceDirectionTheta,a+1,maxLength/nBits*a,
												mapPtr,firstElementPtr,&firstElementPtr[b]);
					}
				}
			}

		}
		

		if(*shipObjPtr!=NULL){
			lastPosition=position;
			position=*sourcePosition;
			directionTheta=*sourceDirectionTheta;

			if(fireMode!=0){
				position.x+=maxLength*globalScale;
				position=position.translate(-*sourcePosition);
				position=position.rotateZAxis(directionTheta/RADIAN*-1);
				position=position.translate(*sourcePosition);
				
				totalVelocity=(position-lastPosition)/dt;
				VectorT vDirection=position-*sourcePosition;

				VectorT rayColor=color*.2;

				VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);
				mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.1,
					fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

				if(CHAOTIC_MODE){
					float d=vDirection.norm();
					if(d!=0){
						velColor=(vDirection/d+1.0)/2.0;
						velColor.y=1.0-velColor.y;
						mapPtr->addImpulseToBuffer(impulsePosition,velColor,.1,
												fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
					}
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
