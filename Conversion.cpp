#include "Conversion.h"

string numberToString(int n){
	string r,t;

	if(n<0){
		t+='-';
		n*=-1;
	}
		
	if(n/10!=0) r=numberToString(n/10);
	t+=r;
	t+=(n-n/10*10+'0');

	return t;
}


VectorT valueToRedGreen(float n){
	if(n<0) n=0;
	VectorT color(0,0,0);
	if(n>1.0) n-=(int)n;
	int block=(int)(n/.5);
	float v=(n-(block*.5f))/.5f;
	switch(block){
		case 0: color=VectorT(1,v,0); break;
		case 1: color=VectorT(1-v,1,0); break;
		case 2: color=VectorT(0,1,0); break;
	}
	return color;
}

VectorT HSVToRGB(VectorT hsv){
	int i;
	float f, p, q, t;
	float h,s,v;
	float r,g,b;
	h=hsv.x;
	s=hsv.y;
	v=hsv.z;

	if( s == 0 ) {
		// achromatic (grey)
		return VectorT(v,v,v);
	}

	h /= 60;			// sector 0 to 5
	i = (int)floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
	return VectorT(r,g,b);

}