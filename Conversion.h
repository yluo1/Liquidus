#ifndef _CONVERSION_
#define _CONVERSION_

#include <string>
#include <vector>

#include "VectorT.h"

using namespace std;

string numberToString(int n);

VectorT valueToRedGreen(float n);
VectorT HSVToRGB(VectorT hsv);

#endif