#include "ForcePongShaders.h"
	
char explosionFragment[]="																									\n\
uniform vec2 origin;																										\n\
uniform float radius;																										\n\
uniform sampler2D tex;																										\n\
uniform float texScaled;																									\n\
uniform float impulseStrength;																								\n\
uniform bool implosion;																										\n\
																															\n\
void main(){																												\n\
																															\n\
	vec2 diff=gl_TexCoord[0].st-origin;																						\n\
	float distSq=dot(diff,diff);																							\n\
	float t=radius*radius*texScaled*texScaled;																				\n\
																															\n\
	if(distSq<t){																											\n\
		if(implosion) diff=diff*-1.0;																						\n\
		float frac=1.0-distSq/t;																							\n\
		vec3 dir=vec3((normalize(diff)+1.0)/2.0,texture2D(tex,gl_TexCoord[0].st).z);										\n\
		gl_FragColor=vec4(impulseStrength*frac*dir+texture2D(tex,gl_TexCoord[0].st).xyz*(1.0-impulseStrength*frac),1.0);	\n\
	}else{																													\n\
		gl_FragColor=vec4(texture2D(tex,gl_TexCoord[0].st).xyz,1.0);														\n\
	}																														\n\
}																															\n\
";