#ifndef _VECTORT_
#define _VECTORT_

#include <stdio.h>
#include <math.h>

class VectorT{
public:
	float x,y,z,w;

	//////////////Constructors
	VectorT(){
		x=y=z=0.0f;
		w=1.0f;
	}
	VectorT(float x,float y){
		this->x=x;
		this->y=y;
		this->z=0;
		w=1.0;
	}
	VectorT(float x,float y,float z){
		this->x=x;
		this->y=y;
		this->z=z;
		w=1.0;
	}
	
	VectorT(float x,float y,float z,float w){
		this->x=x;
		this->y=y;
		this->z=z;
		this->w=w;
	}

	//////////////Functions
	float norm(){
		return sqrtf(x*x+y*y+z*z);
	}
	float normSquared(){
		return (float)(x*x+y*y+z*z);
	}
	void normalizeSelf(){
		float n=norm();
		if(n==0) n=1;
		x/=n;
		y/=n;
		z/=n;
	}

	VectorT normalize(){
		float n=norm();
		if(n==0) n=1;
		return VectorT(x/n,y/n,z/n);
	}

	float dot(VectorT v){
		return x*v.x+y*v.y+z*v.z;
	}

	VectorT cross(VectorT v){
		return VectorT(y*v.z-z*v.y,-(x*v.z-z*v.x),x*v.y-y*v.x);
	}

	VectorT projection(VectorT v){//Project v onto this
		return VectorT(x,y,z)*dot(v)/normSquared();
	}

	bool clockwiseTestZ(VectorT v){//Test along z axis
		return (cross(v).z>0?0:1);
	}

	float angle(VectorT v){//Angle between two vectors
		return acos(dot(v)/(norm()*v.norm()));
	}

	VectorT translate(VectorT v){
		return VectorT(x+v.x,y+v.y,z+v.z);
	}

	VectorT scale(float v){
		return VectorT(x*v,y*v,z*v);
	}

	VectorT scale(VectorT v){
		return VectorT(x*v.x,y*v.y,z*v.z);
	}


	VectorT rotateZAxis(float theta){
		return VectorT(	x*cosf(theta)+y*sinf(theta),
						x*-sinf(theta)+y*cosf(theta),
						z,
						w);
	}

	VectorT rotateYAxis(float theta){
		return VectorT(	x*cosf(theta)+z*-sinf(theta),
						y,
						x*sinf(theta)+z*cosf(theta),
						w);
	}

	VectorT rotateXAxis(float theta){
		return VectorT(	x,
						y*cosf(theta)+z*sin(theta),
						y*-sinf(theta)+z*cosf(theta),
						w);
	}

	VectorT rotateArbitraryAxis(VectorT axis,float theta){
		float x=axis.x,y=axis.y,z=axis.z;
		float c=cosf(theta);
		float s=sinf(theta);
		float t=1-c;
    
		float arbMatrix[4][4]={{t*x*x+c,t*x*y-s*z,t*x*z+s*y,0},
					 {t*x*y+s*z,t*y*y+c, t*y*z-s*x,0},
					 {t*x*z-s*y, t*y*z+s*x, t*z*z+c,0},
					 {0,0,0,1}};

		return transformVectorT(arbMatrix);
	}

	VectorT transformVectorT(float m[4][4]){
		return VectorT(x*m[0][0]+y*m[1][0]+z*m[2][0]+w*m[3][0],
					  x*m[0][1]+y*m[1][1]+z*m[2][1]+w*m[3][1],
					  x*m[0][2]+y*m[1][2]+z*m[2][2]+w*m[3][2],
					  x*m[0][3]+y*m[1][3]+z*m[2][3]+w*m[3][3]);
	}

	void print(){
		printf("%f %f %f %f\n",x,y,z,w);
	}

	//////////Unary operators
	VectorT operator-(){
		return VectorT(-x,-y,-z,-w);
	}

	//////////Binary operators
	void operator=(VectorT v){
		x=v.x;
		y=v.y;
		z=v.z;
		w=v.w;
	}

	void operator=(float v){
		x=y=z=w=v;
	}
	
	VectorT operator+(VectorT v){
		return VectorT(x+v.x,y+v.y,z+v.z,w+v.w);
	}

	VectorT operator+(float v){
		return VectorT(x+v,y+v,z+v,w+v);
	}

	VectorT operator-(VectorT v){
		return VectorT(x-v.x,y-v.y,z-v.z,w-v.w);
	}

	VectorT operator-(float v){
		return VectorT(x-v,y-v,z-v,w-v);
	}

	VectorT operator*(VectorT v){
		return VectorT(x*v.x,y*v.y,z*v.z,w*v.w);
	}

	VectorT operator*(float v){
		return VectorT(x*v,y*v,z*v,w*v);
	}

	VectorT operator/(VectorT v){
		return VectorT(x/v.x,y/v.y,z/v.z,w/v.w);
	}

	VectorT operator/(float v){
		return VectorT(x/v,y/v,z/v,w/v);
	}

	void operator+=(VectorT v){
		x+=v.x;
		y+=v.y;
		z+=v.z;
		w+=v.w;
	}

	void operator+=(float v){
		x+=v;
		y+=v;
		z+=v;
		w+=v;
	}

	void operator-=(VectorT v){
		x-=v.x;
		y-=v.y;
		z-=v.z;
		w-=v.w;
	}

	void operator-=(float v){
		x-=v;
		y-=v;
		z-=v;
		w-=v;
	}

	void operator*=(VectorT v){
		x*=v.x;
		y*=v.y;
		z*=v.z;
		w*=v.w;
	}

	void operator*=(float v){
		x*=v;
		y*=v;
		z*=v;
		w*=v;
	}
	void operator/=(VectorT v){
		x/=v.x;
		y/=v.y;
		z/=v.z;
		w/=v.w;
	}

	void operator/=(float v){
		x/=v;
		y/=v;
		z/=v;
		w/=v;
	}

	bool operator==(VectorT v){
		return (x==v.x&&y==v.y&&z==v.z&&w==v.w);
	}
	
	bool operator!=(VectorT v){
		return !(x==v.x&&y==v.y&&z==v.z&&w==v.w);
	}

};
#endif