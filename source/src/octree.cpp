#include "project.h"

OctreeNode *octree;

void OctreeNode::insert(Body *body, bool move)
{
    if(!move)
    {
        ++vertices_count;
        this->bodies.push_back(body);
        body->nodes.push_back(this);
        mass += body->mass;
        mg += body->p * body->mass;
    }

    if(leaf) // the current node doesn't have any child
    {
        // if the limit is reached, divide the current node.
        if(vertices_count > 1) this->divide();
    }

    else // current node already has children
    {
        // find the appropriate child for the given entity and insert it
        OctreeNode *node = children[((body->p.z > midpoint.z) | ((body->p.y > midpoint.y) << 1 ) | ((body->p.x > midpoint.x) << 2 ))];
        node->insert(body);
    }
}
    
void OctreeNode::divide()
{
    if(!this->leaf) return;
    leaf = false;

    for(int i = 0; i < 8; ++i)
    {
        OctreeNode *node = new OctreeNode();
        node->size = this->size/2.0;
        node->mass = 0;

        node->midpoint = vec(
            midpoint.x + ((i & 4) ? node->size/2.0 : -node->size/2.0),
            midpoint.y + ((i & 2) ? node->size/2.0 : -node->size/2.0),
            midpoint.z + ((i & 1) ? node->size/2.0 : -node->size/2.0)
        );

        node->depth = this->depth + 1;
        node->parent = this;

        children[i] = node;
    }

    for(unsigned int i = 0; i < this->bodies.size(); ++i)
    {
        this->insert(this->bodies[i], true);
    }
}

void OctreeNode::compute_center_of_mass()
{
    this->g = this->mg / this->mass;
    if(!this->leaf)
    {
        for(int i = 0; i < 8; ++i)
        {
            this->children[i]->compute_center_of_mass();
        }
    }
}

extern OctreeNode *octree;

int build_octree()
{
    if(!octree)
        octree = new OctreeNode();

    vec bmin = (1, 1, 1), bmax = (-1, -1, -1);

    for(unsigned int i = 0; i < bodies.size(); ++i)
    {
        if(bodies[i]->p.x < bmin.x) bmin.x = bodies[i]->p.x;
        if(bodies[i]->p.y < bmin.y) bmin.y = bodies[i]->p.y;
        if(bodies[i]->p.z < bmin.z) bmin.z = bodies[i]->p.z;

        if(bodies[i]->p.x > bmax.x) bmax.x = bodies[i]->p.x;
        if(bodies[i]->p.y > bmax.y) bmax.y = bodies[i]->p.y;
        if(bodies[i]->p.z > bmax.z) bmax.z = bodies[i]->p.z;
    }

    vec delta = bmax - bmin;

    octree->size = max(delta.x, max(delta.y, delta.z));
    octree->midpoint = (bmin+bmax)/2.0;
    
    for(unsigned int i = 0; i < bodies.size(); ++i)
    {
        bodies[i]->nodes.clear();
        octree->insert(bodies[i]);
    }
    
    octree->compute_center_of_mass();
    return 1;
}