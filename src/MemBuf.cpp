// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: Easier R embedding into C++
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "MemBuf.h"

extern bool verbose;
extern const char *programName;

MemBuf::~MemBuf() {
    if (verbose) std::cout << "MemBuf::dtor BEGIN" << std::endl;
    free(p_m);
    if (verbose) std::cout << "MemBuf::dtor END" << std::endl;
}

MemBuf::MemBuf(int sizebytes) {
    if (verbose) std::cout << "MemBuf::ctor BEGIN" << std::endl;
    p_m = (membuf_st*) malloc(sizebytes+sizeof(struct membuf_st));

    if (p_m == NULL) {
	fprintf(stderr,"%s: init_membuf() failed! Exiting!!!\n\n", programName);
	exit(1);
	//return NULL; /* unreached */
    }

    p_m->size = sizebytes;
    p_m->count = 0;
    p_m->buf = (unsigned char *)p_m+sizeof(struct membuf_st);
    
    //return lb;
    if (verbose) std::cout << "MemBuf::ctor END" << std::endl;
}

void MemBuf::resize() {		// Use power of 2 resizing 
    membuf_t lb = p_m;
    lb = p_m = (membuf_st*) realloc(lb, lb->size*2+sizeof(struct membuf_st));
    if (lb == NULL) {
	fprintf(stderr,"%s: init_membuf() failed! Exiting!!!\n\n", programName);
	exit(1);
    }
    lb->size =  lb->size * 2;
    lb->buf = (unsigned char *)lb+sizeof(struct membuf_st);
}

void MemBuf::rewind(){
    p_m->count = 0;
}

void MemBuf::add(char *buf){
    membuf_t mb = p_m;
    int buflen = strlen(buf);

    while ((buflen + (mb->count)) >= mb->size){
	resize();
    }

    memcpy(mb->buf + mb->count, buf, buflen);
    mb->buf[mb->count+buflen] = '\0';
    mb->count += buflen;
}

