#define C_CELERITY (299792458.0)
#define C_GRAVITY (6.6784e-11)
#define C_SUNMASS (1.989e30)
#define C_SUNCONST (1.32712440018e20) // C_GRAVITY*C_SUNMASS, known with a high precision.

#define GALAXY_SIZE 1e13

#define TIMESTEP 0.10

extern std::vector<Body *> bodies;
extern vec set_min_bounds, set_max_bounds;

void generate_bodies_exp(int amount, double size, double mass, vec g, vec vg);
void generate_bodies_ring(int amount, double radius, double size, double mass);
void move_bodies(double dt);