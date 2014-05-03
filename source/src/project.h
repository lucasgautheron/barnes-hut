#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <vector>
#include <sstream>
#include <stdarg.h>
#include <math.h>
#include <random>
#include <algorithm>
#include <ctime>

#if defined(WIN32)
//    #include <ppl.h>
    #include <windows.h>
    #include <Mmsystem.h>
    #include <time.h>
#else
    #include <sys/time.h>
#endif

#ifdef GUI
#include <gl/gl.h>
#include <gl/glu.h>

#include <SDL/SDL.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#endif

#include "zlib/zlib.h"

using namespace std;

#include "tools.h"
#include "stream.h"
#include "maths.h"
#include "octree.h"
#include "physics.h"
#ifdef GUI
#include "render.h"
#endif
