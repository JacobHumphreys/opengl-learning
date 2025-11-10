#pragma once

#include "debug.hpp"
#include "GL/glew.h"
#include <string>

/* Clears errors, Calls x, and inserts a break point on error */
#define CALL_GL(x) clearGlErrors();\
    x;\
    ASSERT_BP(logGlCall(#x,__FILE__, __LINE__));\

void clearGlErrors();

bool logGlCall(std::string function, std::string file, int line);
