#include "project.h"

long stream::size()
{
    long pos = tell(), endpos;
    if(pos < 0 || !seek(0, SEEK_END)) return -1;
    endpos = tell();
    return pos == endpos || seek(pos, SEEK_SET) ? endpos : -1;
}

bool stream::getline(char *str, int len)
{
    loop(i, len-1)
    {
        if(read(&str[i], 1) != 1) { str[i] = '\0'; return i > 0; }
        else if(str[i] == '\n') { str[i+1] = '\0'; return true; }
    }
    if(len > 0) str[len-1] = '\0';
    return true;
}

// filestream

filestream::filestream() : file(NULL) {}
filestream::~filestream() { close(); }

bool filestream::open(const char *name, const char *mode)
{
    if(file) return false;
    file = fopen(name, mode);
//#ifdef __linux__
#if 0
    struct statvfs buf;
    if(file && strchr(mode,'w'))
    {
        int fail = fstatvfs(fileno(file), &buf);
        if (fail || (unsigned long long)buf.f_frsize * (unsigned long long)buf.f_bavail < MINFSSIZE)
        {
            close();
            return false;
        }
    }
#endif
    return file!=NULL;
}

bool filestream::opentemp(const char *name, const char *mode)
{
    if(file) return false;
#ifdef WIN32
    file = fopen(name, mode);
#else
    file = tmpfile();
#endif
    return file!=NULL;
}

void filestream::close()
{
    if(file) { fclose(file); file = NULL; }
}

bool filestream::end() { return feof(file)!=0; }
long filestream::tell() { return ftell(file); }
bool filestream::seek(long offset, int whence) { return fseek(file, offset, whence) >= 0; }
int filestream::read(void *buf, int len) { return (int)fread(buf, 1, len, file); }
int filestream::write(const void *buf, int len) { return (int)fwrite(buf, 1, len, file); }
int filestream::getchar() { return fgetc(file); }
bool filestream::putchar(int c) { return fputc(c, file)!=EOF; }
bool filestream::putint(int n)
{
    if(n<128 && n>-127) put(n);
    else if(n<0x8000 && n>=-0x8000) { put(0x80); put(n); put(n>>8); }
    else { put(0x81); put(n); put(n>>8); put(n>>16); put(n>>24); }
    return true;
}
bool filestream::getline(char *str, int len) { return fgets(str, len, file)!=NULL; }
bool filestream::putstring(const char *str) { return fputs(str, file)!=EOF; }

int filestream::printf(const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    int result = vfprintf(file, fmt, v);
    va_end(v);
    return result;
}

// gzstream

gzstream::gzstream() : file(NULL), buf(NULL), reading(false), writing(false), autoclose(false), crc(0), headersize(0)
{
    zfile.zalloc = NULL;
    zfile.zfree = NULL;
    zfile.opaque = NULL;
    zfile.next_in = zfile.next_out = NULL;
    zfile.avail_in = zfile.avail_out = 0;
}

gzstream::~gzstream()
{
    close();
}

void gzstream::writeheader()
{
    uchar header[] = { MAGIC1, MAGIC2, Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_UNIX };
    file->write(header, sizeof(header));
}

void gzstream::readbuf(int size)
{
    if(!zfile.avail_in) zfile.next_in = (Bytef *)buf;
    size = min(size, int(&buf[BUFSIZE] - &zfile.next_in[zfile.avail_in]));
    int n = file->read(zfile.next_in + zfile.avail_in, size);
    if(n > 0) zfile.avail_in += n;
}

int gzstream::readbyte(int size)
{
    if(!zfile.avail_in) readbuf(size);
    if(!zfile.avail_in) return 0;
    zfile.avail_in--;
    return *(uchar *)zfile.next_in++;
}

void gzstream::skipbytes(int n)
{
    while(n > 0 && zfile.avail_in > 0)
    {
        int skipped = min(n, (int)zfile.avail_in);
        zfile.avail_in -= skipped;
        zfile.next_in += skipped;
        n -= skipped;
    }
    if(n <= 0) return;
    file->seek(n, SEEK_CUR);
}

bool gzstream::checkheader()
{
    readbuf(10);
    if(readbyte() != MAGIC1 || readbyte() != MAGIC2 || readbyte() != Z_DEFLATED) return false;
    int flags = readbyte();
    if(flags & F_RESERVED) return false;
    skipbytes(6);
    if(flags & F_EXTRA)
    {
        int len = readbyte(512);
        len |= readbyte(512)<<8;
        skipbytes(len);
    }
    if(flags & F_NAME) while(readbyte(512));
    if(flags & F_COMMENT) while(readbyte(512));
    if(flags & F_CRC) skipbytes(2);
    headersize = file->tell() - zfile.avail_in;
    return zfile.avail_in > 0 || !file->end();
}

bool gzstream::open(stream *f, const char *mode, bool needclose, int level)
{
    if(file) return false;
    for(; *mode; mode++)
    {
        if(*mode=='r') { reading = true; break; }
        else if(*mode=='w') { writing = true; break; }
    }
    if(reading)
    {
        if(inflateInit2(&zfile, -MAX_WBITS) != Z_OK) reading = false;
    }
    else if(writing && deflateInit2(&zfile, level, Z_DEFLATED, -MAX_WBITS, min(MAX_MEM_LEVEL, 8), Z_DEFAULT_STRATEGY) != Z_OK) writing = false;
    if(!reading && !writing) return false;

    autoclose = needclose;
    file = f;
    crc = crc32(0, NULL, 0);
    buf = new uchar[BUFSIZE];

    if(reading)
    {
        if(!checkheader()) { stopreading(); return false; }
    }
    else if(writing) writeheader();
    return true;
}

uint gzstream::getcrc() { return crc; }

void gzstream::finishreading()
{
    if(!reading) return;
    /*if(dbggz)
    {
        uint checkcrc = 0, checksize = 0;
        loopi(4) checkcrc |= uint(readbyte()) << (i*8);
        loopi(4) checksize |= uint(readbyte()) << (i*8);
        if(checkcrc != crc)
            conoutf("gzip crc check failed: read %X, calculated %X", checkcrc, crc);
        if(checksize != zfile.total_out)
            conoutf("gzip size check failed: read %d, calculated %d", checksize, zfile.total_out);
    }*/
}

void gzstream::stopreading()
{
    if(!reading) return;
    inflateEnd(&zfile);
    reading = false;
}

void gzstream::finishwriting()
{
    if(!writing) return;
    for(;;)
    {
        int err = zfile.avail_out > 0 ? deflate(&zfile, Z_FINISH) : Z_OK;
        if(err != Z_OK && err != Z_STREAM_END) break;
        flush();
        if(err == Z_STREAM_END) break;
    }
    uchar trailer[8] =
    {
        crc&0xFF, (crc>>8)&0xFF, (crc>>16)&0xFF, (crc>>24)&0xFF,
        zfile.total_in&0xFF, (zfile.total_in>>8)&0xFF, (zfile.total_in>>16)&0xFF, (zfile.total_in>>24)&0xFF
    };
    file->write(trailer, sizeof(trailer));
}

void gzstream::stopwriting()
{
    if(!writing) return;
    deflateEnd(&zfile);
    writing = false;
}

void gzstream::close()
{
    if(reading) finishreading();
    stopreading();
    if(writing) finishwriting();
    stopwriting();
    DELETEA(buf);
    if(autoclose) DELETEP(file);
}

bool gzstream::end() { return !reading && !writing; }
long gzstream::tell() { return reading ? zfile.total_out : (writing ? zfile.total_in : -1); }

bool gzstream::seek(long offset, int whence)
{
    if(writing || !reading) return false;

    if(whence == SEEK_END)
    {
        uchar skip[512];
        while(read(skip, sizeof(skip)) == sizeof(skip));
        return !offset;
    }
    else if(whence == SEEK_CUR) offset += zfile.total_out;

    if(offset >= (int)zfile.total_out) offset -= zfile.total_out;
    else if(offset < 0 || !file->seek(headersize, SEEK_SET)) return false;
    else
    {
        if(zfile.next_in && zfile.total_in <= uint(zfile.next_in - buf))
        {
            zfile.avail_in += zfile.total_in;
            zfile.next_in -= zfile.total_in;
        }
        else
        {
            zfile.avail_in = 0;
            zfile.next_in = NULL;
        }
        inflateReset(&zfile);
        crc = crc32(0, NULL, 0);
    }

    uchar skip[512];
    while(offset > 0)
    {
        int skipped = min(offset, (long)sizeof(skip));
        if(read(skip, skipped) != skipped) { stopreading(); return false; }
        offset -= skipped;
    }

    return true;
}

const char *findfile(const char *name, const char *mode)
{
    return name;
}

int gzstream::read(void *buf, int len)
{
    if(!reading || !buf || !len) return 0;
    zfile.next_out = (Bytef *)buf;
    zfile.avail_out = len;
    while(zfile.avail_out > 0)
    {
        if(!zfile.avail_in)
        {
            readbuf(BUFSIZE);
            if(!zfile.avail_in) { stopreading(); break; }
        }
        int err = inflate(&zfile, Z_NO_FLUSH);
        if(err == Z_STREAM_END) { crc = crc32(crc, (Bytef *)buf, len - zfile.avail_out); finishreading(); stopreading(); return len - zfile.avail_out; }
        else if(err != Z_OK) { stopreading(); break; }
    }
    crc = crc32(crc, (Bytef *)buf, len - zfile.avail_out);
    return len - zfile.avail_out;
}

bool gzstream::flush()
{
    if(zfile.next_out && zfile.avail_out < BUFSIZE)
    {
        if(file->write(buf, BUFSIZE - zfile.avail_out) != int(BUFSIZE - zfile.avail_out))
            return false;
    }
    zfile.next_out = buf;
    zfile.avail_out = BUFSIZE;
    return true;
}

int gzstream::write(const void *buf, int len)
{
    if(!writing || !buf || !len) return 0;
    zfile.next_in = (Bytef *)buf;
    zfile.avail_in = len;
    while(zfile.avail_in > 0)
    {
        if(!zfile.avail_out && !flush()) { stopwriting(); break; }
        int err = deflate(&zfile, Z_NO_FLUSH);
        if(err != Z_OK) { stopwriting(); break; }
    }
    crc = crc32(crc, (Bytef *)buf, len - zfile.avail_in);
    return len - zfile.avail_in;
}

int gzstream::printf(const char *fmt, ...)
{
    char d[MAXSTRLEN];
    memset(d, 0, MAXSTRLEN);
    va_list v;
    va_start(v, fmt);
    _vsnprintf(d, MAXSTRLEN, fmt, v); d[MAXSTRLEN-1] = 0;
    int result = write((void *)d, strlen(d));
    va_end(v);
    return result;
}

filestream *openrawfile(const char *filename, const char *mode)
{
    const char *found = findfile(filename, mode);
    if(!found) return NULL;
    filestream *file = new filestream();
    if(!file->open(found, mode))
    {
        delete file; return NULL;
    }
    return file;
}

filestream *opentempfile(const char *name, const char *mode)
{
    const char *found = findfile(name, mode);
    filestream *file = new filestream();
    if(!file->opentemp(found ? found : name, mode)) { delete file; return NULL; }
    return file;
}

gzstream *opengzfile(const char *filename, const char *mode, stream *file, int level)
{
    stream *source = file ? file : openrawfile(filename, mode);
    if(!source) return NULL;
    gzstream *gz = new gzstream();
    if(!gz->open(source, mode, !file, level)) { if(!file) delete source; return NULL; }
    return gz;
}

int getfilesize(const char *filename)
{
    filestream *f = openrawfile(filename, "rb");
    if(!f) return -1;
    int len = f->size();
    delete f;
    return len;
}

int readstream(stream *s, std::string &str)
{
    char buf[1024];
    int len = 0;
    int read = 0;
    while( (read = s->read(buf, sizeof(buf))) > 0)
    {
        len += read;
        buf[read-1] = '\0';
        str += buf;
    }
    return len;
}