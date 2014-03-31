#ifndef _CHAOSMISSILEOBJECT_
#define _CHAOSMISSILEOBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "MissileObject.h"

class ChaosMissileObject:public MissileObject{
private:
	int spread;
	int maxSpread;
	int splitCount;
public:
	
	ChaosMissileObject(){
	}

	ChaosMissileObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		int spread,int maxSpread,int splitCount,
		Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:MissileObject(objID,teamID,type,color,position,initialVelocity,radius,mass,damage,life,hitPoints,
		mapPtr,firstElementPtr,currentElementPtr){

		this->color=color;
		this->spread=spread;
		this->maxSpread=maxSpread;
		this->splitCount=splitCount;

		blowOnContact=false;
	}

	virtual ~ChaosMissileObject(){
	}

	virtual void display(){

		VectorT color=this->color*.2;
		glColor4f(color.x,color.y,color.z,(float)(maxSpread-spread)/maxSpread*.5+.5);

		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		glTranslatef(-position.x,-position.y,-position.z);

		glBegin(GL_TRIANGLES);
		glVertex3f(position.x,position.y,position.z);
		glVertex3f(position.x-radius/2.0,position.y+radius/4.0,position.z);
		glVertex3f(position.x-radius/2.0,position.y-radius/4.0,position.z);
		glEnd();
		glPopMatrix();

	
	}

	virtual void update(){
		processAcceleration();

		//Update kinetic physics
		totalVelocity=velocityRelative;
		processBoundary();
		position+=(totalVelocity)*(dt)*globalScale;

		life-=dt;

		if(hitPoints<=0||life<=0){//Life end so spawn bits
			if(spread<maxSpread){
				for(int nSplits=0;nSplits<splitCount;++nSplits){
					int a=findNextOpenObject(firstElementPtr,objCount);
					if(a!=-1){
						int spreadRange=360;
						VectorT vel=VectorT(cosf(directionTheta+fabs((rand()%spreadRange-spreadRange/2)/180.0*PI)),
											sinf(directionTheta+fabs((rand()%spreadRange-spreadRange/2)/180.0*PI)),0)*1000/(spread+1);
						firstElementPtr[a]=new ChaosMissileObject(a,teamID,type,color,
							position,vel,
							(radius+15)*1.5,mass/2.0f,damage,initialLife*.25+initialLife*.6*(rand()%1000/1000.0f),hitPoints,spread+1,maxSpread,splitCount,
							mapPtr,firstElementPtr,&firstElementPtr[a]);
					}
				}
			}

			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
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
		if(spread==0) mapPtr->addImpulseToBuffer(impulsePosition,VectorT(1,1,1,1),.7,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		else mapPtr->addImpulseToBuffer(impulsePosition,(-origColor+1)*.6+this->color*.4,.7,fluidRadius,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
		if(initialLife-life>initialLife*.1){
			if(CHAOTIC_MODE) mapPtr->addImpulseToBuffer(impulsePosition,color,.2,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
		}

	}
	
};
#endif
