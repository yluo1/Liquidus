#include "ForcePongShaders.h"

char vorticityFragment[]="\n\
uniform sampler2D texVelocity;\n\
uniform float texScaled;\n\
uniform float dxy;\n\
void main(void){\n\
	vec2 pos=gl_TexCoord[0].st;\n\
	//pos=(floor(pos/texScaled)+0.5)*texScaled;\n\
	vec2 leftPix=texture2D(texVelocity,vec2(pos.x-texScaled,pos.y)).xy;\n\
	vec2 rightPix=texture2D(texVelocity,vec2(pos.x+texScaled,pos.y)).xy;\n\
	vec2 bottomPix=texture2D(texVelocity,vec2(pos.x,pos.y-texScaled)).xy;\n\
	vec2 topPix=texture2D(texVelocity,vec2(pos.x,pos.y+texScaled)).xy;\n\
	float curl=.5/dxy*((rightPix.y-leftPix.y)-(topPix.x-bottomPix.x));\n\
	gl_FragColor=vec4(curl,20.0*curl,20.0*-curl,0);\n\
}\n\
";
