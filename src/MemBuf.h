
class MemBuf {			// simple C++-ification of littler's membuf
private:
    typedef struct membuf_st {
	int size;
	int count;
	unsigned char *buf;
    } *membuf_t;
    membuf_t p_m;
public:    
    MemBuf(int sizebytes=1024);
    ~MemBuf();
    void resize();
    void rewind();
    void add(char *buf);
    unsigned char* getBufPtr() { return p_m->buf; };
};
