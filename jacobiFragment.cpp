#include "ForcePongShaders.h"

char jacobiFragment[]="														\n\
uniform sampler2D tex;														\n\
uniform sampler2D tex2;														\n\
uniform float texScaled;													\n\
uniform float alpha;														\n\
uniform float rBeta;														\n\
																			\n\
void main(void){															\n\
																			\n\
	vec2 pos=gl_TexCoord[0].st;												\n\
	//pos=(floor(pos/texScaled)+0.5)*texScaled;\n\
	vec2 cPix=texture2D(tex2,pos).xy;										\n\
																			\n\
	vec2 leftPix=texture2D(tex,vec2(pos.x-texScaled,pos.y)).xy;				\n\
	vec2 rightPix=texture2D(tex,vec2(pos.x+texScaled,pos.y)).xy;			\n\
	vec2 bottomPix=texture2D(tex,vec2(pos.x,pos.y-texScaled)).xy;			\n\
	vec2 topPix=texture2D(tex,vec2(pos.x,pos.y+texScaled)).xy;				\n\
																			\n\
	vec2 fColor=(leftPix+rightPix+bottomPix+topPix+alpha*cPix).xy*rBeta;	\n\
																			\n\
	gl_FragColor=vec4(fColor,0.0,1.0);										\n\
}																			\n\
";
