// main.cpp
// program start / quit
#include "project.h"

void init();
void close();
void callback_quit();

bool keyreleased = true;
bool mousedown = false;

int mouse_x = 0, mouse_y = 0;

int lastmillis = 0, curmillis = 0, diffmillis;

bool paused = false;
int main(int argc, char *argv[])
{
    init();

    generate_bodies_exp(800, GALAXY_SIZE, 3e30, vec(-GALAXY_SIZE*3, GALAXY_SIZE*0.1, 0), vec(+2.5e6, 0, 0));
    generate_bodies_exp(400, GALAXY_SIZE, 1e30, vec(GALAXY_SIZE*3, 0, 0), vec(-2.5e6, 0, 0));
    //generate_bodies_ring(1000, 2e11, 1.2e11, 1e10);

    build_octree();

    // main loop
#ifdef GUI
    SDL_Event event;
    for(;;)
    {
        lastmillis = curmillis; 
        curmillis = SDL_GetTicks();
        diffmillis = curmillis - lastmillis;

        SDL_PollEvent(&event);

        bool pressed = false;
        switch(event.type)
        {
            case SDL_KEYDOWN:
            {
                pressed = true; // a key has been pressed
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: callback_quit(); break;
                    case SDLK_p: if(keyreleased) paused = !paused; break;
                    case SDLK_RIGHT:
                    {
                        angle.y -= 20.0*double(diffmillis)/1000.0;
                        angle.y = angle.y > 360 ? int(diffmillis) % 360 : angle.y;
                    }
                    break;
                    case SDLK_LEFT:
                    {
                        angle.y += 20.0*double(diffmillis)/1000.0;
                        angle.y = angle.y > 360 ? int(angle.y) % 360 : angle.y;
                    }
                    case SDLK_UP:
                    {
                        angle.z += 20.0*double(diffmillis)/1000.0;
                        angle.z = angle.z > 360 ? int(angle.z) % 360 : angle.z;
                    }
                    break;
                    case SDLK_DOWN:
                    {
                        angle.z -= 20.0*double(diffmillis)/1000.0;
                        angle.z = angle.z > 360 ? int(angle.z) % 360 : angle.z;
                    }
                    break;
                    case SDLK_KP_PLUS:
                    {
                        zoom *= (1-double(diffmillis)/1000.0);
                    }
                    break;
                    case SDLK_KP_MINUS:
                    {
                        zoom *= (1+double(diffmillis)/1000.0);
                    }
                    break;
                    default:
                    {
                        
                    }
                }
            }
            break;
            case SDL_QUIT: callback_quit(); break;
            case SDL_VIDEORESIZE:
            {
                w = event.resize.w;
                h = event.resize.h;
                glViewport(0, 0, w, h);

            }
        }
        
        if(pressed && keyreleased) keyreleased = false;
        else if(!pressed && !keyreleased) keyreleased = true;
        
        render();
        
        move_bodies(10000);
        DELETEP(octree);
        build_octree();
        

        if(diffmillis < 2) SDL_Delay(2-diffmillis);
    }
#else

#endif
    callback_quit();
    return EXIT_SUCCESS;
}

void init()
{
    srand ( (unsigned int)time(NULL) );
    dbgoutf("Initializating...");
#ifdef GUI
    init_sdl();
#endif
}

void callback_quit()
{
#ifdef GUI
    TTF_Quit();
    SDL_Quit();
#endif
    exit(EXIT_SUCCESS);
}