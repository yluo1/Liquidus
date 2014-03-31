#ifndef _MODEL_
#define _MODEL_

#include <iostream>
#include <fstream>
#include <vector>

#include "GL/glew.h"
#include "GL/glut.h"

#include "PolygonT.h"

using namespace std;

class Model{
private:
	int polyCountMax;
	PolygonT *poly;
public:
	int polyCount;

	Model(){
		polyCount=0;
		polyCountMax=0;
		poly=NULL;
	}

	Model(char *fileName){
		readModel(fileName);
	}

	~Model(){
	}

	void readModel(char *fileName){
		if(poly!=NULL){
			free(poly);
			poly=NULL;
			polyCountMax=0;
			polyCount=0;
		}

		ifstream is;
		is.open(fileName,ios::in|ios::binary);
		
		is.read((char*)&polyCount,sizeof(polyCount));
		is.read((char*)&polyCountMax,sizeof(polyCountMax));

		poly=(PolygonT*)calloc(1,sizeof(PolygonT)*polyCountMax);

		for(int a=0;a<polyCount;++a){
			is.read((char*)(poly+a),sizeof(PolygonT));
		}
		is.close();
	}

	void writeModel(char *fileName){
		ofstream os;
		os.open(fileName,ios::out|ios::binary);
		
		os.write((char*)&polyCount,sizeof(polyCount));
		os.write((char*)&polyCountMax,sizeof(polyCountMax));

		for(int a=0;a<polyCount;++a){
			os.write((char*)(poly+a),sizeof(PolygonT));
		}
		os.close();
	}

	void addPoly(PolygonT p){
		if(polyCount>=polyCountMax){//Reallocate memory
			polyCountMax=polyCountMax*2+1;
			poly=(PolygonT*)realloc(poly,polyCountMax*sizeof(PolygonT));
		}
		poly[polyCount++]=p;
	}

	int replacePoly(PolygonT p, int n){
		if(n>=polyCount) return 0;
		poly[n]=p;
		return 1;
	}

	int removePoly(int n){
		if(n>=polyCount||n<0) return 0;
		for(int a=n;a<polyCount-1;++a) poly[a]=poly[a+1];
		--polyCount;
		return 1;
	}

	int removePoly(PolygonT p){
		for(int a=0;a<polyCount;++a){
			if(poly[a]==p){
				removePoly(a);
				return 1;
			}
		}
		return 0;
	}

	void reset(){
		cleanup();
		polyCount=polyCountMax=0;

	}
	void cleanup(){
		if(poly!=NULL) free(poly);
		poly=NULL;
	}

	void display(){
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);

		for(int a=0;a<polyCount;++a){
			poly[a].display();
		}
		glPopAttrib();
	}

	void display(float alpha){//Display with maximum alpha
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);

		for(int a=0;a<polyCount;++a){
			poly[a].display(alpha);
		}
		glPopAttrib();
	}

	float findClosestPoint(VectorT p,VectorT *closestPoint){

		float dist=INT_MAX,c;
		for(int a=0;a<polyCount;++a){
			for(int b=0;b<3;++b){
				if((c=(p-poly[a].vertex[b]).norm())<dist){
					dist=c;
					*closestPoint=poly[a].vertex[b];
				}
			}
		}
		return dist;
	}

	void generateSample(char *fileName){
		reset();
		addPoly(PolygonT(VectorT(0,1),VectorT(-1,0),VectorT(0,0),VectorT(0,0,1)));
		addPoly(PolygonT(VectorT(0,-1),VectorT(-1,0),VectorT(0,0),VectorT(0,0,1)));
		addPoly(PolygonT(VectorT(1,0),VectorT(-.707,.707),VectorT(-.707,-.707),VectorT(0,0,0)));
		writeModel(fileName);
	}
};

#endif