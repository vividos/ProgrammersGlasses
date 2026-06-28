// teststaticlib.cpp : Defines the functions for the static library.
//

#include "pch.h"

// This is an example of a library function
__declspec(dllexport)
int teststaticlib()
{
   return 42;
}
