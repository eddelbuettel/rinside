// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Simple example showing an overly complicated way to do the standard 'hello, world' example
//
// Copyright (C) 2009 Dirk Eddelbuettel 
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois
//
// GPL'ed 

#include "RInside.h"                    // for the embedded R via RInside

int main(int argc, char *argv[]) {

    RInside R(argc, argv);              // create an embedded R instance 
    
    std::string txt = "Hello, world!\n";// assign a standard C++ string to 'txt'
    R["txt"] = txt;			// assign C++ string var to R variable 'txt'

    std::string evalstr = "cat(txt)";
    R.parseEvalQ(evalstr);              // eval the init string, ignoring any returns

    exit(0);
}

