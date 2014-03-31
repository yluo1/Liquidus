#include "ForcePongShaders.h"

char advectionFragment[]="																							\n\
uniform float deltaTime;																							\n\
uniform sampler2D texVelocity;																						\n\
uniform sampler2D texAdvect;																						\n\
uniform float texScaled;																							\n\
uniform float dxy;																									\n\
uniform float dissipation;																							\n\
vec2 backTrack();																									\n\
vec3 billinearInterpolate(vec2 pos);																				\n\
void main(void){																									\n\
	vec3 texColor=billinearInterpolate(backTrack());																\n\
	gl_FragColor=vec4(dissipation*(.5-texColor)+texColor,1.0);														\n\
}																													\n\
																													\n\
vec2 backTrack(){//Returns the prev location																		\n\
	return gl_TexCoord[0].st-(texture2D(texVelocity,gl_TexCoord[0].st).xy-.5)*2.0*(1.0/dxy)*deltaTime;					\n\
}																													\n\
																													\n\
vec3 billinearInterpolate(vec2 pos){																				\n\
	vec2 pixPos=pos/texScaled;																						\n\
	vec2 diff=pixPos-floor(pixPos);																					\n\
																													\n\
	vec3 leftPix=texture2D(texAdvect,vec2(pos.x-texScaled,pos.y)).xyz;												\n\
	vec3 rightPix=texture2D(texAdvect,vec2(pos.x+texScaled,pos.y)).xyz;												\n\
	vec3 bottomPix=texture2D(texAdvect,vec2(pos.x,pos.y-texScaled)).xyz;											\n\
	vec3 topPix=texture2D(texAdvect,vec2(pos.x,pos.y+texScaled)).xyz;												\n\
																													\n\
	return vec3(.5*(((1.0-diff.x)*leftPix+diff.x*rightPix)+((1.0-diff.y)*bottomPix+diff.y*topPix)));				\n\
}																													\n\
";