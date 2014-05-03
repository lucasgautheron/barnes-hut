// variable types

enum
{
    VTYPE_INT = 0,
    VTYPE_FLOAT,
    VTYPE_DOUBLE,
    VTYPE_VEC,
    VTYPE_STR
};

struct variable
{
    char *name;
    short int type;
    int refcount;

    union
    {
        int *i;
        float *f;
        double *d;
        vec *v;
        char **s;
    } val;

    variable() : type(VTYPE_INT), refcount(0), name(NULL) { }
};