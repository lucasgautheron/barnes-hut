class stream
{
public:
    virtual ~stream() {}
    virtual void close() = 0;
    virtual bool end() = 0;
    virtual long int tell() { return -1; }
    virtual bool seek(long offset, int whence = SEEK_SET) { return false; }
    virtual long int size();
    virtual int read(void *buf, int len) { return 0; }
    virtual std::string getstring()
    {
        std::string str;
        char buf[1024];
        int len = 0;
        int r = 0;
        while( (r = read(buf, sizeof(buf))) > 0)
        {
            len += r;
            buf[r-1] = '\0';
            str += buf;
        }
        return str;
    }
    virtual int write(const void *buf, int len) { return 0; }
    virtual int getchar() { uchar c; return read(&c, 1) == 1 ? c : -1; }
    virtual bool putchar(int n) { uchar c = n; return write(&c, 1) == 1; }
    virtual bool putint(int n)
    {
        if(n<128 && n>-127) put(n);
        else if(n<0x8000 && n>=-0x8000) { put(0x80); put(n); put(n>>8); }
        else { put(0x81); put(n); put(n>>8); put(n>>16); put(n>>24); }
        return true;
    }
    virtual bool getline(char *str, int len);
    virtual bool putstring(const char *str) { int len = (int)strlen(str); return write(str, len) == len; }
    virtual bool putline(const char *str) { return putstring(str) && putchar('\n'); }
    virtual int printf(const char *fmt, ...) { return -1; }
    virtual uint getcrc() { return 0; }

    template<class T> bool put(T n) { return write(&n, sizeof(n)) == sizeof(n); }
    template<class T> bool putlil(T n) { return put<T>(lilswap(n)); }
    template<class T> bool putbig(T n) { return put<T>(bigswap(n)); }

    template<class T> T get() { T n; return read(&n, sizeof(n)) == sizeof(n) ? n : 0; }
    template<class T> T getlil() { return lilswap(get<T>()); }
    template<class T> T getbig() { return bigswap(get<T>()); }
};

class filestream : public stream
{
private:
    FILE *file;
public:
    filestream();
    ~filestream();

    bool open(const char *name, const char *mode);
    bool opentemp(const char *name, const char *mode);

    void close();
    bool end();
    long int tell();
    bool seek(long offset, int whence);
    int read(void *buf, int len);
    int write(const void *buf, int len);
    int getchar();
    bool putchar(int c);
    bool putint(int n);
    bool getline(char *str, int len);
    bool putstring(const char *str);
    int printf(const char *fmt, ...);
};

class gzstream : public stream
{
private:
    stream *file;
    z_stream zfile;
public:
    enum
    {
        MAGIC1   = 0x1F,
        MAGIC2   = 0x8B,
        BUFSIZE  = 16384,
        OS_UNIX  = 0x03
    };

    enum
    {
        F_ASCII    = 0x01,
        F_CRC      = 0x02,
        F_EXTRA    = 0x04,
        F_NAME     = 0x08,
        F_COMMENT  = 0x10,
        F_RESERVED = 0xE0
    };
    uchar *buf;
    bool reading, writing, autoclose;
    uint crc;
    int headersize;

    gzstream();
    ~gzstream();

    void writeheader();
    void readbuf(int size = BUFSIZE);
    int readbyte(int size = BUFSIZE);
    void skipbytes(int n);
    bool checkheader();
    bool open(stream *f, const char *mode, bool needclose, int level);
    uint getcrc();
    void finishreading();
    void stopreading();
    void finishwriting();
    void stopwriting();
    void close();
    bool end();
    long int tell();
    bool seek(long offset, int whence);
    int read(void *buf, int len);
    bool flush();
    int write(const void *buf, int len);
};

const char *findfile(const char *name, const char *mode);
filestream *openrawfile(const char *filename, const char *mode);
filestream *opentempfile(const char *name, const char *mode);
gzstream *opengzfile(const char *filename, const char *mode, stream *file = NULL, int level = Z_BEST_COMPRESSION);
int getfilesize(const char *filename);
int readstream(stream *s, std::string &str);