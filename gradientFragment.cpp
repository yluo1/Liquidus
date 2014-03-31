#include "ForcePongShaders.h"

char gradientFragment[]="																	\n\
uniform sampler2D texVelocity;																\n\
uniform sampler2D texPressure;																\n\
uniform float texScaled;																	\n\
uniform float dxy;																			\n\
																							\n\
void main(void){																			\n\
	vec2 pos=gl_TexCoord[0].st;																\n\
	//pos=(floor(pos/texScaled)+0.5)*texScaled;\n\
	vec3 cPix=texture2D(texVelocity,pos).xyz;												\n\
	float leftPix=texture2D(texPressure,vec2(pos.x-texScaled,pos.y)).x;						\n\
	float rightPix=texture2D(texPressure,vec2(pos.x+texScaled,pos.y)).x;					\n\
	float bottomPix=texture2D(texPressure,vec2(pos.x,pos.y-texScaled)).x;					\n\
	float topPix=texture2D(texPressure,vec2(pos.x,pos.y+texScaled)).x;						\n\
																							\n\
	gl_FragColor=vec4(cPix.xy-.5/dxy*vec2(rightPix-leftPix,topPix-bottomPix),0.0,1.0);		\n\
																							\n\
}																							\n\
";