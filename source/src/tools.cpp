// tools.cpp
// different tools
#include "project.h"

// debug message
void dbgoutf(const char *format, ...)
{
    defvformatstring(d, format, format);
    printf("%s\n", d);
}

float human_readable_size(int bytes, char &mul)
{
    const char* prefixes = "KMGT";
    float current = float(bytes);
    char p = 0;
    while(current > 1000 && (p = *prefixes++)) current /= 1000.0f;
    mul = p;
    return current;
}