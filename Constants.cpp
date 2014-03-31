#include "Constants.h"

float defaultDt=1.0;

float* initDefaultSinValues(int size){
	float* table=(float*)calloc(size,sizeof(float));
	for(int a=0;a<size;++a) table[a]=sinf((float)a/size*PI*2);
	return table;
}
