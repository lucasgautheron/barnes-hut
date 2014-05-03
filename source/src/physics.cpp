#include "project.h"

std::vector<Body *> bodies;

void generate_bodies_exp(int amount, double size, double mass, vec g, vec vg)
{
    std::default_random_engine generator;
    std::exponential_distribution<double> exp_distribution(1.0);
    std::uniform_real_distribution<double> uniform_distribution(0, 1.0);
    std::uniform_int_distribution<int> uniform_idistribution(0, 1);

    Body *blackhole = new Body();
    blackhole->mass = mass * 100000;
    blackhole->p = g;
    blackhole->v = vg;
    bodies.push_back(blackhole);

    for(int i = 0; i < amount; ++i)
    {
        Body *body = new Body();
        body->mass = 1e30;
        double r = size * exp_distribution(generator);
            r = min(r, size * 6);
        double phi = 2 * C_PI * uniform_distribution(generator);
        double z = size * exp_distribution(generator) / 20.0;
            z = min(z, size * 6 / 20);
        if(uniform_idistribution(generator)) z = -z;

        body->p = vec(r * cos(phi), r * sin(phi), z) + g;
        body->v.cross((body->p-g), vec(0, 0, 1));
        body->v.normalize(sqrt(C_GRAVITY * blackhole->mass / sqrt(r*r+z*z)));
        body->v += vg;
        //body->v = vec(0, 0, 0);
        bodies.push_back(body);
    }
}

void generate_bodies_ring(int amount, double radius, double size, double mass)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uniform_distribution(0, 1.0);
    std::uniform_int_distribution<int> uniform_idistribution(0, 1);

    Body *sun = new Body();
    sun->p = vec(0, 0, 0);
    sun->v = vec(0, 0, 0);
    sun->mass = C_SUNMASS;
    bodies.push_back(sun);

    Body *jupiter = new Body();
    jupiter->p = vec(8.1662e11, 0, 0);
    jupiter->v = vec(0, -1.244e4, 0);
    jupiter->mass = 1.8986e27;
    bodies.push_back(jupiter);

    for(int i = 0; i < amount; ++i)
    {
        Body *body = new Body();
        body->mass = 1e10;
        double r = radius + size * uniform_distribution(generator);
        double phi = 2 * C_PI * uniform_distribution(generator);

        body->p = vec(r * cos(phi), r * sin(phi), 0);
        body->v.cross(body->p, vec(0, 0, 1));
        body->v.normalize(sqrt(C_GRAVITY * sun->mass / r));
        //body->v = vec(0, 0, 0);
        bodies.push_back(body);
    }
}

void compute_gravity(Body *b, OctreeNode *node)
{
    if(!node || !node->bodies.size()) return;
    if(node->leaf)
    {
        if(node->bodies.size() && node->bodies[0] != b) b->a += (node->g - b->p) * node->mass / (pow((node->g - b->p).norm(), 3.0));
    }
    else
    {
        if(std::find(b->nodes.begin(), b->nodes.end(), node)==b->nodes.end() // if node does not contain the current body and is small enough
            && node->size < 0.1 * (b->a - node->g).norm())                   // then the gravity can be applied
        {
            b->a += (node->g - b->p) * node->mass / (pow((node->g - b->p).norm(), 3.0));
        }
        else
        {
            for(unsigned int i = 0; i < 8; ++i) compute_gravity(b, node->children[i]);
        }
    }
}

void move_bodies(double dt)
{
    for(unsigned int i = 0; i < bodies.size(); ++i)
    {
        bodies[i]->a = vec(0, 0, 0);

        compute_gravity(bodies[i], octree);
        
        bodies[i]->a.mul(C_GRAVITY);
        bodies[i]->v = bodies[i]->v + bodies[i]->a * dt;
        bodies[i]->p = bodies[i]->p + bodies[i]->v * dt;
    }
}