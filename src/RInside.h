// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.h: R/C++ interface class library -- Easier R embedding into C++
//
// Copyright (C) 2009 - 2010 Dirk Eddelbuettel
//
// This file is part of RInside.
//
// RInside is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// RInside is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RInside.  If not, see <http://www.gnu.org/licenses/>.

#include <string>
#include <vector>
#include <iostream>

#include <R.h>
#include <Rembedded.h>
#include <Rversion.h>
#include <Rdefines.h>
#ifndef WIN32
#define R_INTERFACE_PTRS
#include <Rinterface.h>
#endif
#include <R_ext/Parse.h>
#include <R_ext/RStartup.h>

#include "MemBuf.h"

class RInside {
private:
    MemBuf mb_m;

    bool verbose_m;				// private switch

    void init_tempdir(void);
    void init_rand(void);
    void autoloads(void);

public:
    int parseEval(const std::string & line, SEXP &ans);
    int parseEvalQ(const std::string & line);

    void assign(const std::vector< std::vector< double > > & mat, const std::string & nam);
    void assign(const std::vector< std::vector< int > > & mat, const std::string & nam);
    void assign(const std::vector< std::string > & vec, const std::string & nam);
    void assign(const std::vector< double > & vec, const std::string & nam);
    void assign(const std::vector< int > & vec, const std::string & nam);
    void assign(const std::string & txt, const std::string & nam);

    RInside(const int argc, const char* const argv[]);
    ~RInside();
};

// simple logging help
inline void logTxtFunction(const char* file, const char* line, const char* expression, const bool verbose) {
    if (verbose) {
	std::cout << file << ":" << line << " expression: " << expression << std::endl;
    }
}

#ifndef logTxT
//#define logTxt(x, b) logTxtFunction(__FILE__, __LINE__, x, b);
#define logTxt(x, b) 
#endif
