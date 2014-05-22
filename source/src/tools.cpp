// tools.cpp
// different tools
#include "project.h"

// debug message
void dbgoutf(const char *format, ...)
{
    defvformatstring(d, format, format);
    printf("%s\n", d);
}

bool export_bodies(const char *file)
{
    filestream *fp = openrawfile(file, "w+");
    if(!fp)
        return false;
    for(unsigned int i = 0; i < bodies.size(); ++i)
    {
        fp->printf("%d|%e|%e;%e;%e|%e;%e;%e\n", i, bodies[i]->mass, bodies[i]->p.x, bodies[i]->p.y, bodies[i]->p.z, bodies[i]->v.x, bodies[i]->v.y, bodies[i]->v.z);
    }
    fp->close();
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