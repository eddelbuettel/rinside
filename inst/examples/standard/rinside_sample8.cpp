// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Simple example showing how to use R[] = ; 
//
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois and GPL'ed 

#include "RInside.h"                    // for the embedded R via RInside

int main(int argc, char *argv[]) {

    RInside R(argc, argv);              // create an embedded R instance 
    
    R["x"] = 10 ;
    R["y"] = 20 ;

    R.parseEvalQ("z <- x + y") ;
    int sum = Rcpp::as<int>( R["z"] ); 
  
    std::cout << "10 + 20 = " << sum << std::endl ; 
    exit(0);
}

