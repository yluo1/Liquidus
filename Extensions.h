#ifndef _EXTENSIONS_
#define _EXTENSIONS_

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> 
#include "GL/glew.h"


typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int); // Windows specfic to enable vsync turn off
extern PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

#endif