/*          compilation, code writing tools          */

#ifndef WIN32
#define _vsnprintf vsnprintf
#endif

#ifdef NULL
#undef NULL
#endif
#define NULL 0

// pointers
#define DELETEP(p) { if(p) delete p; p = NULL; }
#define DELETEA(a) { if(a) delete[] a; a = NULL; }
#define DELETEV(v) { while(!v.empty()) delete v.back(), v.pop_back(); }

#define ASSERT(c) if(c) {}

//   custom types
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// rand
inline int rnd(int min, int max) { return min + rand() % (max-min); }
inline int rnd(int max) { return rnd(0, max); }

// loop
#define loop(v,m) for(int v = 0; v<int(m); ++v)
#define looprev(v, m) for(int v = m-1; v>0; --v)
#define loopv(v, ve) loop(v, (ve).length())
#define loopvector(v, ve) loop(v, (ve).size())
/*#define loopv(ve) loop(i, (ve).length())
#define loopvj(ve) loop(j, (ve).length())
#define loopvk(ve) loop(k, (ve).length())
#define loopvrev(v) looprev(i, (ve).length())
#define loopvjrev(v) looprev(j, (ve).length())*/

#define MAXSTRLEN 5000
#define defvformatstring(d,last,fmt) char d[MAXSTRLEN]; { va_list ap; va_start(ap, last); _vsnprintf(d, MAXSTRLEN, fmt, ap); d[MAXSTRLEN-1] = 0; va_end(ap); }

// output
void dbgoutf(const char *format, ...);

// file
float human_readable_size(int bytes, char &mul);
std::string strtoupper(std::string &from);