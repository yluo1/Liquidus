#ifndef _TELEPORTLASERIIOBJECT_
#define _TELEPORTLASERIIOBJECT_

#include "VectorT.h"
#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"

#include "ForcePongConstants.h"
#include "TeleportLaserObject.h"

class TeleportLaserIIObject:public TeleportLaserObject{
public:
	Object **shipObjPtr;

	TeleportLaserIIObject(){
	}

	TeleportLaserIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *shipPosition,int step,int fireMode,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:TeleportLaserObject(objID,teamID,type,color,
		position,initialVelocity,radius,damage,life,hitPoints,
		shipObjPtr,shipPosition,step,
		mapPtr,firstElementPtr,currentElementPtr){

		this->shipObjPtr=shipObjPtr;

		if(step==0){
			velocityRelative=velocityRelative.rotateZAxis((fireMode*30)/RADIAN);
			this->initialLife*=1.0+fireMode*.05f;
			this->life=initialLife;
		}
	}
	
	~TeleportLaserIIObject(){
		
	}

	virtual void display(){
		if(step==0){	

			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			
			glColor4f(1,1,1,life/initialLife*.5+.5);
			glPushMatrix();

			glTranslatef(position.x,position.y,position.z);
			glRotatef(directionTheta*RADIAN,0,0,1);
			glScalef(-1,-1.5,1);
			glTranslatef(-position.x,-position.y,-position.z);
		
			float newRad=radius*2;
			glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TRACKING_TELEPORTII].texID);
			glBegin(GL_QUADS);
			glTexCoord2d(1,1); glVertex2f(position.x+newRad,position.y+newRad);
			glTexCoord2d(1,0); glVertex2f(position.x+newRad,position.y-newRad);
			glTexCoord2d(0,0); glVertex2f(position.x-newRad,position.y-newRad);
			glTexCoord2d(0,1); glVertex2f(position.x-newRad,position.y+newRad);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glPopAttrib();
	
			glColor4f(color.x,color.y,color.z,life/initialLife);


			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
			glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);

			newRad=radius;
			glBegin(GL_QUADS);
			 glVertex2f(position.x+newRad,position.y+newRad);
			 glVertex2f(position.x+newRad,position.y-newRad);
			 glVertex2f(position.x-newRad,position.y-newRad);
			 glVertex2f(position.x-newRad,position.y+newRad);
			glEnd();

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			glPopAttrib();

			glPopMatrix();
		}

	}
	virtual void update(){

		if(step==0){		
			//Update kinetic physics
			totalVelocity=velocity+velocityRelative;
			processBoundary();
			position+=(totalVelocity)*(dt)*globalScale;
	
			direction=velocityRelative.normalize();
			directionTheta=atan2(direction.y,direction.x);

			//Add water effects
			VectorT rayColor=color*.3;
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);

			if(CHAOTIC_MODE){
				//Determine Velocity Color
				velColor=velocityRelative.normalize();
				velColor=(velColor+1.0)/2.0;
				velColor.y=1.0-velColor.y;

				VectorT color=velColor;
				if(initialLife-life<initialLife*.1) color=-color+1;
			}
		}else{
			VectorT impulsePosition((position.x)/defaultDisplay.windowWidth,1-(position.y)/defaultDisplay.windowHeight,1.0);

			mapPtr->addImpulseToBuffer(impulsePosition,color,.8,fluidRadius*1.75,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

		}

		life-=dt;
		if(hitPoints<=0||life<=0){//Teleport the ship
			if(step==0&&*shipObjPtr!=NULL){
				VectorT lastPosition=*shipPosition;
				VectorT teleDirection=position-lastPosition;
				VectorT teleMult=teleDirection/TELEPORT_WAVES;
				teleDirection=teleDirection.normalize();

				//*shipPosition=position;
				//Determine Velocity Color
				VectorT wcolor=teleDirection;
				float d=wcolor.norm();
				if(d!=0){
					wcolor=(wcolor+1.0)/2.0;
					wcolor.y=1.0-wcolor.y;
				}

				for(int a=0;a<=TELEPORT_WAVES;++a){//Add waves and make bits
					VectorT pos=lastPosition+teleMult*a;
					VectorT impulsePosition((pos.x)/defaultDisplay.windowWidth,1-(pos.y)/defaultDisplay.windowHeight,1.0);

				//	mapPtr->addImpulseToBuffer(impulsePosition,color,.8,fluidRadius*1.75,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
						
					if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,wcolor,.75,fluidRadius*1.5,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
					
					//Make bits
					int b=findNextOpenObject(firstElementPtr,objCount);
					if(b!=-1){
						firstElementPtr[b]=new TeleportLaserIIObject(b,teamID,type,color,
																	pos,teleDirection,radius*2,damage,dt*2,hitPoints,
																	shipObjPtr,shipPosition,1,0,
																	mapPtr,firstElementPtr,&firstElementPtr[b]);
					}
					
				}
				VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
				mapPtr->addImpulseToBuffer(impulsePosition,color,.5f,
							fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

			}

			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	
};
#endif
