#include "project.h"

#ifdef GUI

SDL_Surface *screen = NULL;
std::vector<GPFont *> fonts;

vec angle(0,0,0);
double zoom = GALAXY_SIZE;
int w = 1024, h = 768;

void init_sdl()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL | SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);

    SDL_WM_SetCaption("Barnes-hut", NULL);
    TTF_Init();
    load_fonts();

}

void load_fonts()
{
    fonts.push_back(new GPFont("consola.ttf", 12));
    //fonts.push_back(new GPFont("arial.ttf", 12));
    //fonts.push_back(new GPFont("consola.ttf", 12));
}

void auto_ortho(double scale, double z_scale)
{
    double x_scale = scale, y_scale = scale;
    if(w>h) y_scale *= double(h)/double(w);
    else x_scale *= double(h)/double(w);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-x_scale, x_scale, -y_scale, y_scale, -z_scale, z_scale);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    auto_ortho(zoom, zoom * 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glRotated(angle.x,1,0,0);
    glRotated(angle.y,0,1,0);
    glRotated(angle.z,0,0,1);


    render_bodies();
    //render_octree(octree);

    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1000, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SDL_GL_SwapBuffers();
}

void draw_text(const char *text, double x, double y, double z, int r, int g, int b, int font)
{
    glPushMatrix();

    glTranslated(x, y, z);
    glColor3ub(r, g, b);

    GPFont *ttf_font = (font >= 0 && font < fonts.size()) ? fonts[font] : fonts.back();
    if(!ttf_font) return;
    ttf_font->Print2D(text);
    glPopMatrix();
}

void draw_label(string text, int x, int y)
{
    int width = text.length() * 9, height = 25;

    draw_text(text.c_str(), (w-width)/2, 10, 0, 255, 255, 255);
}

void render_bodies()
{
    glPointSize(2.0);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBegin(GL_POINTS);
    glColor3d(1.0, 1.0, 1.0);

    for(unsigned int i = 0; i < bodies.size(); ++i)
    {
        glVertex3dv(bodies[i]->p.v);
    }
    
    glEnd();
}

void render_octree(OctreeNode *o)
{
    if(!o->leaf)
    {
        for(int i = 0; i < 8; ++i) render_octree(o->children[i]);
        return;
    }

    if(!o->bodies.size()) return; // decommenter pour afficher les octants feuille seulement

    glColor3f(1.0, 0.0, 0);
    
    vec delta = vec(o->size, o->size, o->size);
    vec inf = o->midpoint-delta/2.0, sup = o->midpoint+delta/2.0;
    vec delta_x = vec(delta.x, 0, 0);
    vec delta_y = vec(0, delta.y, 0);
    vec delta_z = vec(0, 0, delta.z);

    glBegin(GL_LINES);
        glVertex3dv(inf.v); glVertex3dv((inf+delta_x).v);
        glVertex3dv((inf+delta_x).v); glVertex3dv((inf+delta_x+delta_y).v);
        glVertex3dv((inf+delta_x+delta_y).v); glVertex3dv((inf+delta_y).v);
        glVertex3dv((inf+delta_y).v); glVertex3dv((inf).v);

        glVertex3dv((inf+delta_z).v); glVertex3dv((inf+delta_x+delta_z).v);
        glVertex3dv((inf+delta_x+delta_z).v); glVertex3dv((inf+delta_x+delta_y+delta_z).v);
        glVertex3dv((inf+delta_x+delta_y+delta_z).v); glVertex3dv((inf+delta_y+delta_z).v);
        glVertex3dv((inf+delta_y+delta_z).v); glVertex3dv((inf+delta_z).v);

        glVertex3dv((inf).v); glVertex3dv((inf+delta_z).v);
        glVertex3dv((inf+delta_x).v); glVertex3dv((inf+delta_z+delta_x).v);
        glVertex3dv((inf+delta_y).v); glVertex3dv((inf+delta_z+delta_y).v);
        glVertex3dv((inf+delta_x+delta_y).v); glVertex3dv((inf+delta_z+delta_x+delta_y).v);
    glEnd();
}
#endif