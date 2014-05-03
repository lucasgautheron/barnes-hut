// bits : xyz (x = msb, z = lsb). dir+ = 1, dir- = 0
// Eg: x+y-+- = 0b101 => 5
#define OCTDIV 12

struct OctreeNode;

struct Body
{
    vec p, v, a;
    double mass;
    std::vector<OctreeNode *> nodes;
};

struct OctreeNode
{
    vec midpoint;
    double size, mass;
    vec mg, g;
    
    OctreeNode *children[8];
    OctreeNode *parent;
    int depth;
    bool leaf, solid;

    int vertices_count;
    std::vector<Body *> bodies;

    OctreeNode() : size(0), mass(0), parent(NULL), depth(0), leaf(true), vertices_count(0)
    {
        midpoint = mg = g = vec(0, 0, 0);
        for(int i = 0; i < 8; ++i) children[8] = NULL;
        bodies.reserve(1);
    }
    ~OctreeNode()
    {
        if(!leaf) for(int i = 0; i < 8; ++i) if(children[i]) delete children[i];
    }
    void insert(Body *body, bool move = false);
    void divide();
    void computecenterofmass();
};

extern OctreeNode *octree;
int build_octree();
