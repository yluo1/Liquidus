#include "ForcePongShaders.h"

char displayFragment[]="														\n\
uniform sampler2D tex;															\n\
																				\n\
void main(void){																\n\
	gl_FragColor=vec4((texture2D(tex,gl_TexCoord[0].st).xyz),1.0);				\n\
}																				\n\
";
