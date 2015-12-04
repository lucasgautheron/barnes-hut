// consts
#define C_PI (double(3.141592653589793238462643))
#define C_E (double(2.718281828459045235360287))
#define G (double(6.67300e-11))
#define UA (double(1.49597871e11))
#define DAY (double(86400))
#define C (double(299792458))
#define MS (double(1.9891e30))

#define DEG2RAD (C_PI/180.0)
#define RAD2DEG (1/DEG2RAD)

#define random(min, max) (min + (rand() % (int)(max - min + 1)))

// units 
template <typename T>
inline T apply_unit_prefix(T val, char* unit)
{
    switch(*unit)
    {
        case 'G': return val * 1000000000;
        case 'M': return val * 1000000;
        case 'k': return val * 1000;
        case 'h': return val * 100;
        case 'd': return *(unit+1) == 'a' ? val * 10 : val / 10;
        case 'c': return val / 100;
        case 'm': return val / 1000;
        case 'µ': return val / 1000000;
        case 'n': return val / 1000000000;
        case 'p': return val / 1000000000000;
        case 'f': return val / 1000000000000000;
        default: return val;
    }
}

inline unsigned long long int factorial(int n)
{
    unsigned long long int f = 1;
    for(int i = 2; i <= n; ++i) f *= i;
    return f;
}

struct matrix;

// 3-dim vector
struct vec
{
public:
    union
    {
        struct { double x, y, z; };
        double v[3];
        int i[3];
    };

    vec() {x=y=z=0;}
    vec(double a, double b, double c) : x(a), y(b), z(c) {}
    vec(double *v) : x(v[0]), y(v[1]), z(v[2]) {}
    vec(double d) : x(d), y(d), z(d) {}

    bool iszero() const;

    vec &mul(double f);
    vec &div(double f);
    vec &add(double f);
    vec &sub(double f);

    vec &add(const vec &o);
    vec &mul(const vec &o);
    vec &sub(const vec &o);
    vec &div(const vec &o);

    vec &mul(const matrix &m);

    double squaredlen() const;
    double sqrxy() const;

    double dot(const vec &o) const;
    double dotxy(const vec &o) const;

    double norm() const;
    double norm1() const;
    vec &normalize();
    vec &normalize(double n);

    double distxy(const vec &e) const;
    double magnitudexy() const;

    double dist(const vec &e) const;
    double dist(const vec &e, vec &t) const;

    double angle(const vec &e) const;

    bool reject(const vec &o, double max) const;

    vec &cross(const vec &a, const vec &b);
    double cxy(const vec &a);

    void rotate_around_z(double angle);
    void rotate_around_x(double angle);
    void rotate_around_y(double angle);

    vec &rotate(double angle, const vec &d);
    vec &rotate(double c, double s, const vec &d);
    vec &orthogonal(const vec &d);

    double &operator[](int i)       { return v[i]; }
    double  operator[](int i) const { return v[i]; }

    vec  operator+ (vec v) { vec ret(*this); return ret.add(v); }
    vec& operator+=(vec v) { return this->add(v); }
    vec  operator- (vec v) { vec ret(*this); return ret.sub(v); }
    vec& operator-=(vec v) { return this->sub(v); }

    vec  operator* (double f) const { vec ret(*this); return ret.mul(f); }
    vec& operator*=(double f)       { return this->mul(f); }
    vec  operator/ (double f) const { vec ret(*this); return ret.div(f); }
    vec& operator/=(double f)       { return this->div(f); }

    vec operator^ (vec v) const { vec ret(0,0,0); return ret.cross(*this, v); }

    bool operator==(const vec &o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const vec &o) const { return x != o.x || y != o.y || z != o.z; }
    bool operator! ()             const { return iszero(); }
    vec operator-() const { return vec(-x, -y, -z); }
};

struct matrix
{
private:
    int rows, cols;
public:
    std::vector< std::vector <double> > elems;

    matrix()
    {
        resize(3, 3);
        for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j) elems[i][j] = 0.0;
    }
    matrix(vec a, vec b, vec c)
    {
        resize(3, 3);
        for(int j = 0; j < cols; ++j)
        {
            elems[j][0] = a[j];
            elems[j][1] = b[j];
            elems[j][2] = c[j];
        }
    }
    matrix(int r, int c)
    {
        resize(r, c);
        for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j) elems[i][j] = 0.0;
    }
    matrix(const matrix &m)
    {
        resize(m.rows, m.cols);
        for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j) elems[i][j] = m[i][j];
    }
    matrix(const vec &v)
    {
        resize(1, 3);
        for(int j = 0; j < 3; ++j) elems[0][j] = v[j];
    }

    void resize(int r, int c)
    {
        if(r < 1 || c < 1) return;
        rows = r; cols = c;
        elems.resize(rows);
        for(int i = 0; i < rows; ++i) elems[i].resize(cols);
    }

    std::vector<double> &operator[](int i)      { return elems[i]; }
    std::vector<double> operator[](int i) const { return elems[i]; }

    matrix &mul(double f) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] *= f; return *this; }
    matrix &div(double f) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] /= f; return *this; }
    matrix &add(double f) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] += f; return *this; }
    matrix &sub(double f) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] -= f; return *this; }

    matrix &add(const matrix &m) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] += m[i][j]; return *this; }
    matrix &dot(const matrix &m) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] *= m[i][j]; return *this; }
    matrix &sub(const matrix &m) { for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j)  elems[i][j] -= m[i][j]; return *this; }

    matrix &mul(const matrix &a, const matrix &b)
    {
        matrix ret;
        ret.resize(a.rows, b.cols);
        for(int i = 0; i < a.rows; ++i) for(int j = 0; j < b.cols; ++j)
        {
            ret[i][j] = 0.0;
            for(int k = 0; k < a.cols; ++k) ret[i][j] += a[i][k]*b[k][j];
        }
        return (*this = ret);
    }

    matrix &identity(int n)
    {
        resize(n, n);
        for(int i = 0; i < n; ++i) elems[i][i] = 1.0;
        return *this;
    }

    matrix &transpose()
    {
        matrix ret(cols, rows);
        for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j) ret[j][i] = elems[i][j];
        return (*this = ret);
    }

    matrix minus(const int row, const int col)
    {
        matrix res;
        if (row >= 0 && row < rows && col >= 0 && col < cols)
        {
            res = matrix(rows - 1, cols - 1);
            for(int i = 0; i < rows - 1; ++i) for(int j = 0; j < cols - 1; ++j)
                res[i][j] = elems[(i <= row ? i : i+1)][(j <= col ? i : j+1)];
        }
        return res;
    }

    double const trace()
    {
        double s = 0;
        for(int i = 0; i < min(rows, cols); ++i) s += elems[i][i];
        return s;
    }

    // slow det function working with any square matrix
    // if the size is known to be 3x3, use det3x3 instead!
    // http://www.dreamincode.net/forums/topic/55772-determinant-of-matrix-in-c/
    // FIXME: gives wrong results
    inline double det()
    {
        double d = 0.0;

        switch(rows)
        {
            case 1: d = elems[0][0]; break;
            case 2: d = elems[0][0] * elems[1][1] - elems[0][1] * elems[1][0]; break;
            default:
            {
                if(rows <= 0) return 0;
                for(int i = 0; i < rows; ++i)
                {
                    d += (1-((i&0x1)<<2)) * elems[0][i] * this->minus(0, i).det();
                }
            }
            break;
        }

        return d;
    }

    inline int const getrows() { return rows; }
    inline int const getcols() { return cols; }

    matrix  operator+ (matrix m) { matrix ret(*this); return ret.add(m); }
    matrix& operator+=(matrix m) { return this->add(m); }
    matrix  operator- (matrix m) { matrix ret(*this); return ret.sub(m); }
    matrix& operator-=(matrix m) { return this->sub(m); }

    matrix  operator* (double f) const { matrix ret(*this); return ret.mul(f); }
    matrix& operator*=(double f)       { return this->mul(f); }
    matrix  operator/ (double f) const { matrix ret(*this); return ret.div(f); }
    matrix& operator/=(double f)       { return this->div(f); }

    matrix operator* (matrix m) const { matrix ret(*this); return ret.mul(ret, m); }
    matrix operator*= (matrix m)      { return this->mul(*this, m); }

    bool operator==(const matrix &m) const
    {
        if(m.rows != rows || m.cols != cols) return false;
        for(int i = 0; i < rows; ++i) for(int j = 0; j < cols; ++j) if(elems[i][j] != m[i][j]) return false;
        return true;
    }
    bool operator!=(const matrix &m) const { return !(m == *this); }

    void print()
    {
        printf("\n");
        for(int i = 0; i < rows; ++i)
        {
            printf(" [");
            for(int j = 0; j < cols; ++j) printf(" %e ", elems[i][j]);
            printf("]\n");
        }
        printf("\n");
    }

    ~matrix()
    {
        
    }
};

//#define MATRIX_IDENTITY(n) (matrix().identity(n))
#define MATRIX_IDENTITY(n) (matrix(n, n).identity(n))

// rotation matrix (around an axis)
#define ROTATION_MATRIX_X(c, s) (matrix(vec(1, 0, 0),  vec(0, c, -s), vec(0, s, c)))
#define ROTATION_MATRIX_Y(c, s) (matrix(vec(c, 0, s),  vec(0, 1, 0),  vec(-s, 0, c)))
#define ROTATION_MATRIX_Z(c, s) (matrix(vec(c, -s, 0), vec(s, c, 0),  vec(0, 0, 1)))

// rotation matrix
inline matrix &ROTATION_MATRIX(double theta, double phi, double psi)
{
    double ct = cos(theta), cph = cos(phi), cps = cos(psi),
           st = sin(theta), sph = sin(phi), sps = sin(psi);
    
    static matrix m;
    matrix tmp(vec(cph*cps, st*sph*cps + ct*sps, -sph*ct*cps + st*sps),
                    vec(-sps*ct, -sps*sph*st + ct*cps, sps*sph*ct + st*cps),
                    vec(sph, -st*cph, ct*cph));
    m = tmp;
    return m;
}

inline double det3x3(vec &a, vec &b, vec &c)
{
    return (a^b).dot(c);
}

inline double det3x3(matrix &m)
{
    vec a(m[0][0], m[0][1], m[0][2]),
        b(m[1][0], m[1][1], m[1][2]),
        c(m[2][0], m[2][1], m[2][2]);
    return (a^b).dot(c);
}
