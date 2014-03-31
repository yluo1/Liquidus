#include "ForcePongShaders.h"

char impulseFragment[]="																								\n\
uniform vec2 origin;																									\n\
uniform float radius;																									\n\
uniform sampler2D tex;																									\n\
uniform float texScaled;																								\n\
uniform vec3 impulseColor;																								\n\
uniform float impulseStrength;																							\n\
																														\n\
void main(){																											\n\
																														\n\
	vec2 diff=gl_TexCoord[0].st-origin;																					\n\
	float distSq=dot(diff,diff);																						\n\
	float t=radius*radius*texScaled*texScaled;																			\n\
	if(distSq<t){																										\n\
		gl_FragColor=vec4(impulseColor*impulseStrength+texture2D(tex,gl_TexCoord[0].st).xyz*(1.0-impulseStrength),1.0);	\n\
	}else{																												\n\
		gl_FragColor=vec4(texture2D(tex,gl_TexCoord[0].st).xyz,1.0);													\n\
	}																													\n\
}																														\n\
";