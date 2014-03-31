#ifndef _CHAOSMISSILEIIIOBJECT_
#define _CHAOSMISSILEIIIOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"

class ChaosMissileIIIObject:public MissileObject{
private:
	int spreadCycle;
	int maxSpreadCycle;
	int depth;
	int maxSpread;
public:
	
	ChaosMissileIIIObject(){
	}

	ChaosMissileIIIObject(int objID,int teamID,int type,VectorT color,
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
		mass=0;

	}

	virtual ~ChaosMissileIIIObject(){
		
	}

	virtual void display(){
	
		glColor4f(color.x*.25+.75,color.y*.25+.75,color.z*.25+.75,.5);
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ammoTextures[AMMO_CHAOSIII].texID);

		float newRad=radius;
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
		directionTheta+=(depth%2==0?-1:1)*5/RADIAN;

		if(spreadCycle!=-1){
			if(spreadCycle<maxSpreadCycle) ++spreadCycle;
			else{//Propagate
				if(depth<maxSpread){
					int a=findNextOpenObject(firstElementPtr,objCount);
					if(a!=-1){
						firstElementPtr[a]=new ChaosMissileIIIObject(a,teamID,type,color*.8,position,
										velocityRelative,
										(radius+7.5)*2.0f,mass,damage*.5,life,hitPoints,
										maxSpreadCycle,depth+1,maxSpread,
										mapPtr,firstElementPtr,&firstElementPtr[a]);
					}
				}
				spreadCycle=-1;
			}
		}

		float radialFactor=1.5;
		VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
		if(depth>0){
			mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),.4,
							fluidRadius*radialFactor,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION,true);
		}
		
		VectorT position2=position+VectorT(.5-rand()%1000/1000.0f,.5-rand()%1000/1000.0f,0).normalize()*radius*radialFactor;
		VectorT impulseSinkPosition=VectorT(position2.x/defaultDisplay.windowWidth,1-position2.y/defaultDisplay.windowHeight,1.0);	
		if(depth%2==0)	mapPtr->addImpulseToBuffer(impulseSinkPosition,color*.25,.33,
						fluidRadius*.33,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		else			mapPtr->addImpulseToBuffer(impulseSinkPosition,color*.5+.5,.33,
						fluidRadius*.33,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);

		life-=dt;
		if(hitPoints<=0||life<=0){//Life end so spawn bits
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	
};
#endif
