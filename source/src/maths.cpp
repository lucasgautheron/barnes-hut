// maths.cpp
#include "project.h"

// class vec
bool vec::iszero() const { return x==0 && y==0 && z==0; }

vec &vec::mul(double f) { x *= f; y *= f; z *= f; return *this; }
vec &vec::div(double f) { x /= f; y /= f; z /= f; return *this; }
vec &vec::add(double f) { x += f; y += f; z += f; return *this; }
vec &vec::sub(double f) { x -= f; y -= f; z -= f; return *this; }

vec &vec::add(const vec &o) { x += o.x; y += o.y; z += o.z; return *this; }
vec &vec::mul(const vec &o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
vec &vec::sub(const vec &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
vec &vec::div(const vec &o) { x /= o.x; y /= o.y; z /= o.z; return *this; }

vec &vec::mul(const matrix &m)
{
    vec ret = vec(0, 0, 0);
    for(int i = 0; i < 3; ++i) for(int k = 0; k < 3; ++k) ret[i] += m[i][k]*v[k];
    return (*this = ret);
}

double vec::squaredlen() const { return x*x + y*y + z*z; }
double vec::sqrxy() const { return x*x + y*y; }

double vec::dot(const vec &o) const { return x*o.x + y*o.y + z*o.z; }
double vec::dotxy(const vec &o) const { return x*o.x + y*o.y; }

double vec::norm() const { return sqrt(squaredlen()); }
double vec::norm1() const { return abs(x) + abs(y) + abs(z); }
vec &vec::normalize() { div(norm()); return *this; }
vec &vec::normalize(double n) { div(norm()); mul(n); return *this; }

double vec::dist(const vec &e) const { vec t; return dist(e, t); }
double vec::dist(const vec &e, vec &t) const { t = *this; t -= e; return t.norm(); }

double vec::angle(const vec &e) const
{
    return acos(this->dot(e)/(this->norm()*e.norm()));
}

double vec::distxy(const vec &e) const { double dx = e.x - x, dy = e.y - y; return sqrt(dx*dx + dy*dy); }
double vec::magnitudexy() const { return sqrt(x*x + y*y); }

bool vec::reject(const vec &o, double max) const { return x>o.x+max || x<o.x-max || y>o.y+max || y<o.y-max; }

vec &vec::cross(const vec &a, const vec &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }
double vec::cxy(const vec &a) { return x*a.y-y*a.x; }

void vec::rotate_around_z(double angle) { *this = vec(cos(angle)*x-sin(angle)*y, cos(angle)*y+sin(angle)*x, z); }
void vec::rotate_around_x(double angle) { *this = vec(x, cos(angle)*y-sin(angle)*z, cos(angle)*z+sin(angle)*y); }
void vec::rotate_around_y(double angle) { *this = vec(cos(angle)*x-sin(angle)*z, y, cos(angle)*z+sin(angle)*x); }

vec &vec::rotate(double angle, const vec &d)
{
    double c = cos(angle), s = sin(angle);
    return rotate(c, s, d);
}

vec &vec::rotate(double c, double s, const vec &d)
{
    *this = vec(x*(d.x*d.x*(1-c)+c) + y*(d.x*d.y*(1-c)-d.z*s) + z*(d.x*d.z*(1-c)+d.y*s),
                x*(d.y*d.x*(1-c)+d.z*s) + y*(d.y*d.y*(1-c)+c) + z*(d.y*d.z*(1-c)-d.x*s),
                x*(d.x*d.z*(1-c)-d.y*s) + y*(d.y*d.z*(1-c)+d.x*s) + z*(d.z*d.z*(1-c)+c));
    return *this;
}

vec &vec::orthogonal(const vec &d)
{
    int i = fabs(d.x) > fabs(d.y) ? (fabs(d.x) > fabs(d.z) ? 0 : 2) : (fabs(d.y) > fabs(d.z) ? 1 : 2);
    v[i] = d[(i+1)%3];
    v[(i+1)%3] = -d[i];
    v[(i+2)%3] = 0;
    return *this;
}
