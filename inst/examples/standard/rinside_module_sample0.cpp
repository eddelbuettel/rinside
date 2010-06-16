// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Simple example showing how expose a C++ function
//
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois

#include <RInside.h>                    // for the embedded R via RInside

// a c++ function we wish to expose to R
const char* hello( std::string who ){
        std::string result( "hello " ) ;
        result += who ;
        return result.c_str() ;
} 

RCPP_MODULE(bling){
	using namespace Rcpp ;
	function( "hello", &hello );
}

int main(int argc, char *argv[]) {

	// create an embedded R instance
    RInside R(argc, argv);               
        
    // load the bling module
    R["bling"] = LOAD_RCPP_MODULE(bling) ;
    
    // call it and display the result
    std::string result = R.parseEval("bling$hello('world')") ;
    std::cout << "bling$hello( 'world') =  '" << result << "'" << std::endl ; 
    exit(0);
}

