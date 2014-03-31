#ifndef _STRINGBUFFER_
#define _STRINGBUFFER_

#include <iostream>
#include <string>
#include <vector>

#include "GL/glew.h"
#include "GL/glut.h"

#include "Constants.h"
#include "Display.h"
#include "VectorT.h"

using namespace std;

class StringBuffer{
private:
	bool active[MAX_STRING_BUFFER_SIZE];
	string text[MAX_STRING_BUFFER_SIZE];
	int displayTime[MAX_STRING_BUFFER_SIZE];//In number of clicks
	int currentDisplayTime[MAX_STRING_BUFFER_SIZE];
	VectorT position[MAX_STRING_BUFFER_SIZE];
	float size[MAX_STRING_BUFFER_SIZE];
	VectorT color[MAX_STRING_BUFFER_SIZE];
	Display *display;
	int count;

	bool lock;
	VectorT positionLock;

public:
	StringBuffer(Display *display){
		this->display=display;
		if(display==NULL) this->display=&defaultDisplay;

		for(int a=0;a<MAX_STRING_BUFFER_SIZE;++a) active[a]=false;
		count=0;
		lock=false;
	}

	StringBuffer(){
		display=&defaultDisplay;
		for(int a=0;a<MAX_STRING_BUFFER_SIZE;++a) active[a]=false;
		count=0;
		lock=false;
	}
	
	~StringBuffer(){
	}

	void addString(string s,int time,VectorT position,float size,VectorT color){
		int next;
		for(next=0;next<count;++next){
			if(!active[next]) break;
		}

		if(next<MAX_STRING_BUFFER_SIZE){
			active[next]=true;
			text[next]=s;
			displayTime[next]=time;
			currentDisplayTime[next]=time;
			this->position[next]=position;
			this->size[next]=size;
			this->color[next]=color;
			++next;
			count=maxValue(next,count);
		}
	}

	void update(){
		for(int a=count-1;a>=0;--a){
			if(active[a]){
				if(currentDisplayTime[a]<=0){//Deactivate
					active[a]=false;
					text[a].clear();
					if(a==count-1) --count;
				}
				color[a].w=(float)currentDisplayTime[a]/displayTime[a];
				--currentDisplayTime[a];
			}
		}
	}

	void show(){
		int b=0;
		for(int a=0;a<count;++a){
			if(active[a]){
				glColor4fv((GLfloat*)&color[a]);
				if(lock)	display->displayString(text[a],positionLock.x,positionLock.y+b*20,size[a],NULL);
				else		display->displayString(text[a],position[a].x,position[a].y,size[a],NULL);
				++b;
			}
		}
	}

	void setLock(bool lock, int x,int y){
		//Lock will draw strings like text at positionLock origin
		this->lock=lock;
		positionLock.x=x;
		positionLock.y=y;
	}

	void cleanup(){
		for(int a=0;a<count;++a) text[a].clear();
	}
	
};


#endif