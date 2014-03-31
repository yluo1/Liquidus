#ifndef __PARTICLEOBJECT__
#define __PARTICLEOBJECT__

#include "Display.h"
#include "Texture.h"
#include "VectorT.h"

#include "ForcePongConstants.h"

class ParticleObject{
protected:
	float initialLife,life;
	float decay;
	float dt;

	VectorT color;
	int textureId;	//-1 for no texture
	
	VectorT dims;

	VectorT initialPosition,position;
	VectorT initialVelocity,velocity;

	float directionTheta;

	bool respawnOn;

public:
	ParticleObject(){
		
	}

	ParticleObject(	VectorT position,VectorT velocity,
					VectorT dims,
					VectorT color,int texureId,
					float initialLife,float decay,
					bool respawnOn){

		this->position=position;
		initialPosition=position;
		this->velocity=velocity;
		initialVelocity=velocity;
		this->color=color;
		this->initialLife=life=initialLife;
		this->decay=decay;
		this->dims=dims;
		this->respawnOn=respawnOn;
		
		if(textureId>=0) this->textureId=textureId;
		else this->textureId=-1;
		
		dt=deltaTime;

	}

	~ParticleObject(){
	}

	void display(){
		glColor4f(color.x,color.y,color.z,life/initialLife*.5);
		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glRotatef(directionTheta*RADIAN,0,0,1);
		VectorT dims=this->dims*globalScale;

		glBegin(GL_QUADS);
		glVertex2f(dims.x,dims.y);
		glVertex2f(dims.x,-dims.y);
		glVertex2f(-dims.x,-dims.y);
		glVertex2f(-dims.x,dims.y);
		glEnd();

		glPopMatrix();
	}

	void update(){
		float ptDat[4];

		int col=(position.x*(256.0f/defaultDisplay.windowWidth));
		int row=(256.0f-position.y*(256.0f/defaultDisplay.windowHeight));
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

		VectorT v=VectorT(ptDat[0]-.5f,-1*(ptDat[1]-.5f))*2;
		float vMag=v.norm();
		dims.x=1+vMag*5;
		vMag=vMag*2000+1000;
		if(vMag!=0) directionTheta=atan2(v.y,v.x);

		velocity=v*vMag;
		position+=velocity*dt*globalScale;
		position.z=position.z=1;

		//Check particle life end
		if(life<=0){
			if(respawnOn) respawn();
		}

		//Boundary check
		if(position.x<0||position.y<0||position.x>defaultDisplay.windowWidth-1||position.y>defaultDisplay.windowHeight){
			//Particle out of bounds so respawn
			respawn();
		}

		//Decay life
		life-=dt*decay*globalScale;
	}

	void respawn(){
		life=initialLife;
		position=initialPosition;
		velocity=initialVelocity;
	}

};


#endif