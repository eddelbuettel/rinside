// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: Easier R embedding into C++
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 

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
