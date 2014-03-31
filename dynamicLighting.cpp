#include "ForcePongShaders.h"

char dynamicLightFragment[]="\n\
uniform sampler2D inkTex;\n\
uniform sampler2D velTex;\n\
void main(void){\n\
	vec2 pos=gl_TexCoord[0].st;\n\
	vec2 vel=2.0*(texture2D(velTex,pos).xy-.5);\n\
	float velMag=sqrt(dot(vel,vel));\n\
	vec3 norm=vec3(vel.x,vel.y,velMag);\n\
	vec3 ink=texture2D(inkTex,pos).xyz;\n\
	//gl_FragColor=vec4(ink+(1.0-ink)*clamp(velMag-.9,0.0,1.0)+ink*clamp(velMag-.9,-1.0,0.0),1.0);\n\
	gl_FragColor=vec4(ink+(1.0-ink)*norm,1.0);\n\
}\n\
";
