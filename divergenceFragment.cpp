#include "ForcePongShaders.h"

char divergenceFragment[]="\n\
uniform sampler2D tex;\n\
uniform float texScaled;\n\
uniform float dxy;\n\
void main(void){\n\
	vec2 pos=gl_TexCoord[0].st;\n\
	//pos=(floor(pos/texScaled)+0.5)*texScaled;\n\
	vec2 leftPix=texture2D(tex,vec2(pos.x-texScaled,pos.y)).xy;				\n\
	vec2 rightPix=texture2D(tex,vec2(pos.x+texScaled,pos.y)).xy;			\n\
	vec2 bottomPix=texture2D(tex,vec2(pos.x,pos.y-texScaled)).xy;			\n\
	vec2 topPix=texture2D(tex,vec2(pos.x,pos.y+texScaled)).xy;				\n\
	float v=.5/dxy*((rightPix.x-leftPix.x)+(topPix.y-bottomPix.y));\n\
	gl_FragColor=vec4(v,0,0,1);\n\
}\n\
";
