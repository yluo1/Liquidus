#ifndef _TRACKINGLASEROBJECT_
#define _TRACKINGLASEROBJECT_

#include "Constants.h"
#include "ConstantsIO.h"
#include "VectorT.h"

#include "ForcePongConstants.h"
#include "LaserObject.h"

#define TRACKPOINTS 300


class TrackingLaserObject:public LaserObject{

private:
	VectorT *targetVectorT;
	VectorT pts[TRACKPOINTS];
	VectorT velPts[TRACKPOINTS];
	int ptsIndex;
	int ptsEnd;

public:
	
	TrackingLaserObject(){
	}

	TrackingLaserObject(int objID,int teamID,int type,VectorT color,
		VectorT position,VectorT initialVelocity,
		float radius,float mass,float damage,float life,float hitPoints,
		VectorT *targetVectorT,Map *mapPtr,Object **firstElementPtr,Object **currentElementPtr)
		:LaserObject(objID,teamID,type,color,position,initialVelocity,radius,damage,life,hitPoints,true,
		mapPtr,firstElementPtr,currentElementPtr){
		
		this->targetVectorT=targetVectorT;
		this->mass=mass;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->velocityFrameBuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);


		ptsIndex=ptsEnd=0;
		pts[0]=position;

		velPts[0]=VectorT(.5,.5,.5);

	#ifdef LARGETEXTURE_READS

	#endif
		float ptDat[3];
		for(int a=1;a<TRACKPOINTS;++a){
			int col=(pts[a-1].x*(float)(mapPtr->frameBufferWidth)/defaultDisplay.windowWidth);
			int row=(mapPtr->frameBufferHeight-pts[a-1].y*(float)(mapPtr->frameBufferHeight)/defaultDisplay.windowHeight);
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
			VectorT dir=VectorT(ptDat[0]-.5f,ptDat[1]-.5f,0);
	
			pts[a]=pts[a-1]+((*targetVectorT-pts[a-1]).normalize()*10+dir*20)*globalScale;
			velPts[a]=(pts[a]-pts[a-1]).normalize();
			ptsEnd=a;
			if(fabs((pts[a]-*targetVectorT).normSquared())<=25.0) break;//Stop point
		}
		for(int a=ptsEnd;a<TRACKPOINTS;++a) pts[a]=pts[ptsEnd];
		
		//Reset framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

		glPopAttrib();
	
	}
	
	virtual ~TrackingLaserObject(){
	}

	virtual void display(){
		VectorT rayColor=color;
		float alpha=((float)ptsIndex/TRACKPOINTS);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,mapPtr->inkFrameBuffer);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

		glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0,0,mapPtr->frameBufferWidth,mapPtr->frameBufferHeight);


		//Draw head
		glColor4f((1-rayColor.x)*.25,(1-rayColor.y)*.25,(1-rayColor.z)*.25,(1-alpha)*.55);
	
		glBegin(GL_LINE_LOOP);
		for(int a=0;a<15;++a)
			glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/15*a)%SIN_TABLE_SIZE]*radius*alpha,
						position.y+defaultSinValues[SIN_TABLE_SIZE/15*a]*radius*alpha);
		glEnd();


		//Draw tail
		glColor4f(rayColor.x*.25,rayColor.y*.25,rayColor.z*.25,(1-alpha)*.55);
		glBegin(GL_LINE_STRIP);
		for(int a=ptsIndex/2;a<ptsIndex;++a) 
			glVertex3fv((GLfloat*)&pts[a]);

		glEnd();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		if(singleFBO) glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glPopAttrib();

		//Draw head
		glColor4f(color.x,color.y,color.z,.5);
	
		glBegin(GL_LINE_LOOP);
		for(int a=0;a<25;++a)
			glVertex2f(	position.x+defaultSinValues[(SIN_TABLE_SIZE/4+SIN_TABLE_SIZE/25*a)%SIN_TABLE_SIZE]*radius*alpha,
						position.y+defaultSinValues[SIN_TABLE_SIZE/25*a]*radius*alpha);
		glEnd();

	}
	virtual void update(){
		VectorT lastPosition=position;
		position+=(pts[ptsIndex]-position)*dt/DEFAULT_DT;
		ptsIndex++;

		totalVelocity=(position-lastPosition)/dt;

		radius+=.1;
		fluidRadius+=.1*(float)(mapPtr->frameBufferWidth)/defaultDisplay.windowWidth;
		
		VectorT impulsePosition((pts[ptsIndex].x)/defaultDisplay.windowWidth,1-(pts[ptsIndex].y)/defaultDisplay.windowHeight,1.0);
		if(CHAOTIC_MODE){
			if(initialLife-life>initialLife*.05){
				if(totalVelocity.normSquared()!=0){
					VectorT velColor=(totalVelocity.normalize()+1)/2;
					velColor.y=1-velColor.y;
					mapPtr->addImpulseToBuffer(impulsePosition,velColor,.05,fluidRadius,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_DIRECTIONAL);
				}
			}
		}
		life-=dt;
		if(hitPoints<=0||life<=0||ptsIndex>=TRACKPOINTS){//On death
			VectorT impulsePosition(position.x/defaultDisplay.windowWidth,1-position.y/defaultDisplay.windowHeight,1.0);	
			mapPtr->addImpulseToBuffer(impulsePosition,color*.2,.9,
						fluidRadius*2,mapPtr->inkFrameBuffer,3,TYPE_IMPULSE_DIRECTIONAL);
			if(CHAOTIC_MODE){
				mapPtr->addImpulseToBuffer(impulsePosition,VectorT(0,0,0),1.0,
						fluidRadius*2,mapPtr->velocityFrameBuffer,0,TYPE_IMPULSE_EXPLOSION);
			}
			(*currentElementPtr)=NULL;
			delete this;
		}
	}
	
};
#endif
