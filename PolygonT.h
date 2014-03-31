#ifndef _POLYGON_
#define _POLYGON_

#include "GL/glew.h"
#include "GL/glut.h"

#include "VectorT.h"

class PolygonT{
private:
	VectorT texCoord[3];
	VectorT color;
	
	bool textureAttached;
	int texID;

public:
	VectorT vertex[3];
	VectorT normal;

	PolygonT(){
		for(int a=0;a<3;++a){
			vertex[a]=VectorT();
			texCoord[a]=VectorT();
		}
		color=VectorT();
		normal=VectorT(0,0,1);
		textureAttached=false;
	}

	PolygonT(VectorT v0,VectorT v1,VectorT v2,VectorT color){
		vertex[0]=v0;
		vertex[1]=v1;
		vertex[2]=v2;
		this->color=color;
		updateNormal();
		textureAttached=false;
	}
	PolygonT(VectorT v0,VectorT v1,VectorT v2,VectorT t0,VectorT t1,VectorT t2,int texID,VectorT color){
		vertex[0]=v0;
		vertex[1]=v1;
		vertex[2]=v2;
		texCoord[0]=t0;
		texCoord[1]=t1;
		texCoord[2]=t2;
		this->color=color;
		this->texID=texID;
		updateNormal();
		textureAttached=true;
	}

	PolygonT(VectorT *v,VectorT color){
		for(int a=0;a<3;++a) vertex[a]=*(v+a);
		this->color=color;
		updateNormal();
		textureAttached=false;
	}

	PolygonT(VectorT *v,VectorT *t,int texID,VectorT color){
		for(int a=0;a<3;++a){
			vertex[a]=*(v+a);
			texCoord[a]=*(t+a);
		}
		this->color=color;
		this->texID=texID;
		updateNormal();
		textureAttached=true;
	}

	void operator=(PolygonT p){
		for(int a=0;a<3;++a){
			texCoord[a]=p.texCoord[a];
			vertex[a]=p.vertex[a];
		}
		color=p.color;
		texID=p.texID;
		normal=p.normal;
		textureAttached=p.textureAttached;
	}

	bool operator==(PolygonT p){
		return (vertex==p.vertex&&texCoord==p.texCoord&&color==p.color&&textureAttached==p.textureAttached&&texID==p.texID);
	}

	~PolygonT(){
	
	}

	void display(){
		if(textureAttached){
			glColor4fv((GLfloat*)&color);
			glBindTexture(GL_TEXTURE_2D,texID);
			glBegin(GL_TRIANGLES);
			for(int a=0;a<3;++a){
				glTexCoord2fv((GLfloat*)&texCoord[a]);
				glVertex3fv((GLfloat*)&vertex[a]);
			}
			glEnd();
		}else{
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_TEXTURE_2D);
			glColor4fv((GLfloat*)&color);
			glBegin(GL_TRIANGLES);
			for(int a=0;a<3;++a){
				glVertex3fv((GLfloat*)&vertex[a]);
			}
			glEnd();
			glPopAttrib();
		}
	}

	void display(float alpha){
		if(textureAttached){
			glColor4f(color.x,color.y,color.z,color.w*alpha);
			glBindTexture(GL_TEXTURE_2D,texID);
			glBegin(GL_TRIANGLES);
			for(int a=0;a<3;++a){
				glTexCoord2fv((GLfloat*)&texCoord[a]);
				glVertex3fv((GLfloat*)&vertex[a]);
			}
			glEnd();
		}else{
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_TEXTURE_2D);
			glColor4f(color.x,color.y,color.z,color.w*alpha);
			glBegin(GL_TRIANGLES);
			for(int a=0;a<3;++a){
				glVertex3fv((GLfloat*)&vertex[a]);
			}
			glEnd();
			glPopAttrib();
		}
	}


	void updateNormal(){
		normal=(vertex[1]-vertex[0]).cross(vertex[2]-vertex[0]).normalize();
	}
};
#endif