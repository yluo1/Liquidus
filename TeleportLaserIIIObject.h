#ifndef _TELEPORTLASERIIIOBJECT_
#define _TELEPORTLASERIIIOBJECT_

#include "VectorT.h"

#include "Constants.h"
#include "ConstantsIO.h"
#include "ForcePongConstants.h"
#include "TeleportLaserObject.h"

class TeleportLaserIIIObject:public TeleportLaserObject{
private:
	bool isRising;
	VectorT corners[4];
	float rotationSpeed;
	VectorT *shipTarget;
public:
	
	TeleportLaserIIIObject(){
	}

	TeleportLaserIIIObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,float radius,float damage,float life,float hitPoints,
		Object **shipObjPtr,VectorT *shipTarget,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:TeleportLaserObject(objID,teamID,type,color,
		position,initialVelocity,radius,damage,life,hitPoints,
		shipObjPtr,shipPosition,step,
		mapPtr,firstElementPtr,currentElementPtr){

		this->damage=0;
		this->mass=0;
		this->hitPoints=99999;
		this->shipTarget=shipTarget;

		corners[0]=VectorT(position.x+radius,position.y+radius,0);
		corners[1]=VectorT(position.x+radius,position.y-radius,0);
		corners[2]=VectorT(position.x-radius,position.y-radius,0);
		corners[3]=VectorT(position.x-radius,position.y+radius,0);

		rotationSpeed=0;

		blowOnContact=false;
		isRising=true;

	}
	
	~TeleportLaserIIIObject(){
		
	}

	virtual void display(){
	
		VectorT color=this->color*.75+.25;
		if(isRising)	glColor4f(color.x,color.y,color.z,(initialLife-life)/initialLife);
		else			glColor4f(color.x,color.y,color.z,life/initialLife);
		
		glPushAttrib(GL_ENABLE_BIT);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_TRACKING_TELEPORTIII].texID);

		
		glBegin(GL_QUADS);
			for(int a=0;a<4;++a){
				glTexCoord2d(1,1); glVertex2fv((GLfloat*)&corners[0]);
				glTexCoord2d(1,0); glVertex2fv((GLfloat*)&corners[1]);
				glTexCoord2d(0,0); glVertex2fv((GLfloat*)&corners[2]);
				glTexCoord2d(0,1); glVertex2fv((GLfloat*)&corners[3]);
			}
		glEnd();
		

		glPopAttrib();

	}

	virtual void update(){

		if(life<=initialLife*.5) isRising=false;
		
		//Update damage
		damage+=(isRising?.00003:-.00003);
		
		//Update position
		if(*shipObjPtr!=NULL){
			position+=(*shipTarget-position).normalize()*.55;
			corners[0]=VectorT(position.x+radius,position.y+radius,0);
			corners[1]=VectorT(position.x+radius,position.y-radius,0);
			corners[2]=VectorT(position.x-radius,position.y-radius,0);
			corners[3]=VectorT(position.x-radius,position.y+radius,0);
		}

		//Rotate corners
		rotationSpeed+=minValue(damage*damage*333,50);
		if(rotationSpeed>360) rotationSpeed-=360;

		VectorT t1;
		for(int a=0;a<4;++a){
			t1=corners[a].translate(-VectorT(position.x,position.y,position.z));
			t1=t1.rotateZAxis(rotationSpeed);
			t1=t1.translate(VectorT(position.x,position.y,position.z));
			corners[a]=t1;
		}
		VectorT impulsePosition;

		for(int a=0;a<4;++a){
			impulsePosition=VectorT(corners[a].x/defaultDisplay.windowWidth,
							1-corners[a].y/defaultDisplay.windowHeight,1.0);
			VectorT color=this->color*.2;
			color.x+=(1-color.x)/4*a;
			mapPtr->addImpulseToBuffer(impulsePosition,color,.9,
							fluidRadius*.4,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			VectorT inwardVel=((position-corners[a]).normalize()+1)/2;
			inwardVel.y=1-inwardVel.y;
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,inwardVel,.75,
						fluidRadius*.4,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
			}
		}

		life-=dt;
		if(hitPoints<=0||life<=0){//Teleport the ship
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	
};
#endif
