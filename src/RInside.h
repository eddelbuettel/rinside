// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; -*-

#include <string>
#include <vector>
#include <iostream>

#include <R.h>
#include <Rembedded.h>
#include <Rversion.h>
#include <Rdefines.h>
#define R_INTERFACE_PTRS
#include <Rinterface.h>
#include <R_ext/Parse.h>

#include "MemBuf.h"

class RInside {
private:
    MemBuf mb_m;

    bool verbose_m;				// private switch

    void init_tempdir(void);
    void init_rand(void);
    void autoloads(void);

public:
    int parseEval(const std::string line, SEXP &ans);

    void assign(const std::vector< std::vector< double > > & mat, const std::string & nam);
    void assign(const std::string & txt, const std::string & nam);

    RInside(const int argc, const char* const argv[]);
    ~RInside();
};

