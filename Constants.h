#ifndef _CONSTANTS_
#define _CONSTANTS_

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <Math.h>

#include "VectorT.h"

using namespace std;

//Definitions

#define DISPLAYINFO
#define FULLSCREEN		false
#define VSYNC_ON		true

#define GAMESPEED 800
#define WINDOWWIDTH 1024
#define WINDOWHEIGHT 768
#define WINDOWMINCUT .0001
#define WINDOWMAXCUT 1000
#define DISPLAY_2D 0
#define DISPLAY_3D 1
#define FOV 100
#define FONT_SIZE .1

#define PI (3.1415926535897932384626433832795f)
#define RADIAN (180.0/PI)

#define MAX_STRING_BUFFER_SIZE 100
//Macros
#define randomColor() VectorT((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX)
#define minValue(a,b)	((a)<(b)?a:b)
#define maxValue(a,b)	((a)<(b)?b:a)
#define thetaToSinTableIndex(theta,maxTableSize) ((int)((theta)/360.0f*maxTableSize)%(maxTableSize))	



//Externs
extern float defaultDt;

//Globals
float* initDefaultSinValues(int size);

#endif