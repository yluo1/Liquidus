#include "ForcePongShaders.h"

char addVorticityFragment[]="\n\
uniform sampler2D texVelocity;\n\
uniform sampler2D texVorticity;\n\
uniform float texScaled;\n\
uniform float vorticityScale;\n\
uniform float dxy;\n\
uniform float deltaTime;\n\
void main(void){\n\
	vec2 pos=gl_TexCoord[0].st;\n\
	//pos=(floor(pos/texScaled)+0.5)*texScaled;\n\
	float leftPix=texture2D(texVorticity,vec2(pos.x-texScaled,pos.y)).x;\n\
	float rightPix=texture2D(texVorticity,vec2(pos.x+texScaled,pos.y)).x;\n\
	float bottomPix=texture2D(texVorticity,vec2(pos.x,pos.y-texScaled)).x;\n\
	float topPix=texture2D(texVorticity,vec2(pos.x,pos.y+texScaled)).x;\n\
	float cPix=texture2D(texVorticity,pos).x;\n\
	vec2 force=.5/dxy*vec2(abs(topPix-bottomPix),abs(rightPix-leftPix));\n\
	float fMag=sqrt(force.x*force.x+force.y*force.y);//Normalize\n\
	if(fMag>0.0){\n\
		force=force/fMag;\n\
		force=vorticityScale*(cPix)*force;\n\
	}\n\
	gl_FragColor=clamp(vec4(texture2D(texVelocity,pos).xy+force*deltaTime,0,0),0.0,1.0);\n\
}\n\
";