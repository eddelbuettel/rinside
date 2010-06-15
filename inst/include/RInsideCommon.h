// RInsideCommon.h: R/C++ interface class library -- Easier R embedding into C++
//
// Copyright (C) 2010        Dirk Eddelbuettel and Romain Francois
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

#ifndef RINSIDE_RINSIDECOMMON_H
#define RINSIDE_RINSIDECOMMON_H

#include <RInsideConfig.h>

#include <string>
#include <vector>
#include <iostream>

#include <Rcpp.h>

#include <Rembedded.h>
#ifndef WIN32
#define R_INTERFACE_PTRS
#include <Rinterface.h>
#endif
#include <R_ext/RStartup.h>

#include <MemBuf.h>

// simple logging help
inline void logTxtFunction(const char* file, const int line, const char* expression, const bool verbose) {
    if (verbose) {
	std::cout << file << ":" << line << " expression: " << expression << std::endl;
    }
}

#ifdef logTxt
#undef logTxt
#endif
//#define logTxt(x, b) logTxtFunction(__FILE__, __LINE__, x, b);
#define logTxt(x, b)

#endif
