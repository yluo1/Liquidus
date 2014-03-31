#ifndef _TELEPORTLASEROBJECT_
#define _TELEPORTLASEROBJECT_

#include "VectorT.h"

#include "Constants.h"
#include "ConstantsIO.h"
#include "Object.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

#define TELEPORT_WAVES 40

class TeleportLaserObject:public LaserObject{
protected:
	Object **shipObjPtr;
	VectorT *shipPosition;
	int step;
public:
	
	TeleportLaserObject(){
	}

	TeleportLaserObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *shipPosition,int step,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->shipObjPtr=shipObjPtr;
		this->step=step;

		if(step==0){
			this->shipPosition=shipPosition;
			blowOnContact=false;
			mass=.025;
		}
	}

	virtual ~TeleportLaserObject(){
	}
	

	virtual void display(){
		if(step==0){
			
			glColor4f(color.x*.5+.5,color.y*.5+.5,color.z*.5+.5,life/initialLife);
			glPushMatrix();
			glTranslatef(position.x,position.y,position.z);
			glRotatef(directionTheta*RADIAN,0,0,1);
			glTranslatef(-position.x,-position.y,-position.z);

			float newRad=radius;
			glBegin(GL_QUADS);
			glVertex2f(position.x+newRad,position.y+newRad);
			glVertex2f(position.x+newRad,position.y-newRad);
			glVertex2f(position.x-newRad,position.y-newRad);
			glVertex2f(position.x-newRad,position.y+newRad);
			glEnd();

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			VectorT colorOutline=color*.5;
			glColor4f(colorOutline.x,colorOutline.y,colorOutline.z,1.0);
			newRad+=.001;
			glBegin(GL_LINE_LOOP);
			glVertex2f(position.x+newRad,position.y+newRad);
			glVertex2f(position.x+newRad,position.y-newRad);
			glVertex2f(position.x-newRad,position.y-newRad);
			glVertex2f(position.x-newRad,position.y+newRad);
			glEnd();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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
			mapPtr->addImpulseToBuffer(impulsePosition,rayColor,.55,4.0,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);	
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
						firstElementPtr[b]=new TeleportLaserObject(b,teamID,type,color,
																	pos,teleDirection,radius*2,damage*2,dt*2,hitPoints,
																	shipObjPtr,shipPosition,1,
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
