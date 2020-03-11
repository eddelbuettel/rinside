
// RInside_C.cpp: R/C++ interface class library -- Easier R embedding into C
//
// Copyright (C) 2020 -       Lance Bachmeier and Dirk Eddelbuettel
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

#include <RInside.h>

RInside *rr = NULL;

extern "C" {
    void setupRinC() {
        if (rr == NULL)
            rr = new RInside;
    }

    void passToR(SEXP x, char * name) {
        if (rr != NULL)
            rr->assign(x, std::string(name));
    }

    SEXP evalInR(char * cmd) {
        if (rr != NULL)
            return rr->parseEval(std::string(cmd));
        else
            return R_NilValue;
    }

    void evalQuietlyInR(char * cmd) {
        if (rr != NULL)
            rr->parseEvalQ(std::string(cmd));
    }

    void teardownRinC() {
        if (rr != NULL) {
            delete rr;
            rr = NULL;
        }
    }
}
