#ifndef _GENESISLASEROBJECT_
#define _GENESISLASEROBJECT_

#include <Math.h>

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

#define GENESISLASERPOINTS 35

class GenesisLaserObject:public LaserObject{
private:
	int fireMode;
	float maxLength;
	float initialRadius;
	
	Object **shipObjPtr;

	VectorT *sourcePosition;
	VectorT initialSource,initialPosition;
	VectorT rayColor;
public:
	
	GenesisLaserObject(){
	}

	GenesisLaserObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *sourcePosition,float maxLength,int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,
		position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){

		this->sourcePosition=sourcePosition;
		this->fireMode=fireMode;
		this->maxLength=maxLength;
		this->shipObjPtr=shipObjPtr;

		initialSource=*sourcePosition;
		initialPosition=position;
		initialRadius=radius*2;

		blowOnContact=false;
		fluidAffected=false;
		rayColor=VectorT(color.x+(1-color.x)*.85,color.y+(1-color.y)*.85,color.z+(1-color.z)*.85);

		//Determine Velocity Color
		velColor=(-velocityRelative).normalize();
		velColor=(velColor+1.0)/2.0;
		velColor.y=1.0-velColor.y;


	}
	

	virtual void display(){
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
		glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		VectorT color=this->color;
		float newRad=(initialLife-life)/initialLife*radius+radius*((float)fireMode/GENESISLASERPOINTS)*.5;

		glColor4f(color.x,color.y,color.z,life/initialLife);

		glBegin(GL_LINE_LOOP);
		for(int a=0;a<=15;++a)
			glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*newRad*1.3,
						position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*newRad*1.3);
		glEnd();

		glPopAttrib();

	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		
	}

	virtual void update(){
		if(*shipObjPtr!=NULL){
			if(fireMode>0){
				damage*=1.05;
				VectorT lastPosition=position;
				position=(initialPosition-initialSource)+*sourcePosition;
				totalVelocity=(position-lastPosition)/dt;
				
				radius=initialRadius+initialRadius*((float)fireMode/GENESISLASERPOINTS)*.5;
/*
				VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);

				mapPtr->addImpulseToBuffer(impulsePosition,rayColor,(initialLife-life)/initialLife*.1,
					(initialLife-life)/initialLife*(fluidRadius+fluidRadius*((float)fireMode/GENESISLASERPOINTS)*.5),mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

				if(CHAOTIC_MODE){
					VectorT c=velColor;
					if(fireMode>GENESISLASERPOINTS*.1) c=-c+1;
					mapPtr->addImpulseToBuffer(impulsePosition,c,((float)fireMode/GENESISLASERPOINTS)*.33,
												(initialLife-life)/initialLife*(fluidRadius+fluidRadius*((float)fireMode/GENESISLASERPOINTS)*.5),mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
				}
*/
			}else if(fireMode==0){
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->velocityFrameBuffer);
				glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
				glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
				glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

				VectorT lastPosition=position;
				float distMult=maxLength/GENESISLASERPOINTS;
				float ptDat[3];
				float alterateColor=((rand()%1000)/1000.0f)*.95;
				for(int a=1;a<GENESISLASERPOINTS;++a){
		
					int col=(lastPosition.x*(float)(mapPtr->frameBufferWidth)/defaultDisplay.windowWidth);
					int row=(mapPtr->frameBufferHeight-lastPosition.y*(float)(mapPtr->frameBufferHeight)/defaultDisplay.windowHeight);
					if(col>255) col=255;
					else if(col<0) col=0;
					if(row>255) row=255;
					else if(row<0) row=0;

					#ifdef LARGETEXTURE_READS
						ptDat[0]=velocityFrame[3*(256*row+col)];
						ptDat[1]=velocityFrame[3*(256*row+col)+1];
						ptDat[2]=velocityFrame[3*(256*row+col)+2];
					#else
						glReadPixels(col,row,
									1,1,GL_RGB,GL_FLOAT,&ptDat);
					#endif

					VectorT dir=VectorT(ptDat[0]-.5f,ptDat[1]-.5f,0).normalize();
			
					VectorT pos=lastPosition+(velocityRelative.normalize()*(distMult*.75)+dir*(distMult*.25))*globalScale;
					lastPosition=pos;
			
					//Add next bit

					if(pos.x<defaultDisplay.windowWidth&&pos.y<defaultDisplay.windowHeight&&pos.x>=0&&pos.y>=0){//In bounds 
						int b=findNextOpenObject(firstElementPtr,objCount);
						if(b!=-1){
							firstElementPtr[b]=new GenesisLaserObject(b,teamID,type,color*alterateColor+.05,
																		pos,velocityRelative,
																		radius,damage,life,hitPoints,
																		shipObjPtr,sourcePosition,maxLength,1,
																		mapPtr,firstElementPtr,&firstElementPtr[b]);
						}
					}
				}
				glPopAttrib();

				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
				life=0;
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
