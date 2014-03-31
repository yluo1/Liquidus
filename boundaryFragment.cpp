#include "ForcePongShaders.h"

char boundaryFragment[]="																			\n\
uniform sampler2D tex;																				\n\
uniform float texScaled;																			\n\
uniform int scale;																				\n\
uniform vec2 offset;																				\n\
																									\n\
void main(void){																					\n\
	vec2 pos=gl_TexCoord[0].st;\n\
	vec2 offPix;\n\
	if(pos.x<=texScaled||pos.x>1.0-texScaled||pos.y<=texScaled||pos.y>1.0-texScaled){\n\
		if(pos.x<=texScaled) offPix=texture2D(tex,vec2(pos.x+texScaled,pos.y)).xy;\n\
		else if(pos.x>1.0-texScaled) offPix=texture2D(tex,vec2(pos.x-texScaled,pos.y)).xy;\n\
		else if(pos.y<=texScaled) offPix=texture2D(tex,vec2(pos.x,pos.y+texScaled)).xy;\n\
		else offPix=texture2D(tex,vec2(pos.x,pos.y-texScaled)).xy;\n\
		if(scale==1) offPix=1.0-offPix;\n\
	}else offPix=texture2D(tex,pos).xy;\n\
	gl_FragColor=vec4(offPix,0,1);\n\
}\n\
";