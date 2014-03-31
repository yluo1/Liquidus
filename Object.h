#ifndef _OBJECT_
#define _OBJECT_

#include "Constants.h"
#include "VectorT.h"

class Object{
protected:
public:
	VectorT position;
	VectorT velocity;
	VectorT acceleration;
	VectorT force;

	float radius;
	float mass;



	Object **firstElementPtr;//Pointer to the first element of its object array
	Object **currentElementPtr;//Pointer to itself in an object array

	Object(){

	}
	Object(VectorT position,float radius,float mass,Object **firstElementPtr,Object **currentElementPtr){
		this->position=position;
		this->radius=radius;
		this->mass=mass;
		this->firstElementPtr=firstElementPtr;
		this->currentElementPtr=currentElementPtr;
		velocity=VectorT(0,0,0);
		acceleration=VectorT(0,0,0);
		force=VectorT(0,0,0);
	}

	virtual ~Object(){

	}

	virtual void display(){
		glBegin(GL_POINTS);
		glVertex3f(position.x,position.y,position.z);
		glEnd();
	}

	virtual void update(){
		acceleration=(force/mass);
		velocity+=acceleration*(defaultDt);
		position+=velocity*(defaultDt);
	}

	virtual void resize(VectorT windowDims,VectorT oldDims){
		position.x*=(windowDims.x/oldDims.x);
		position.y*=(windowDims.y/oldDims.y);
	}

};
#endif