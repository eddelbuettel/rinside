// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8; -*-
//
// Simple example with data in C++ that is passed to R, processed and a result is extracted
//
// Copyright (C) 2009 Dirk Eddelbuettel 
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois
//
// GPL'ed 

#include "RInside.h"            // for the embedded R via RInside

Rcpp::NumericMatrix createMatrix(const int n) {
    Rcpp::NumericMatrix M(n,n);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            M(i,j) = i*10+j; 
        }
    }
    return(M);
}

int main(int argc, char *argv[]) {
    const int mdim = 4;
    SEXP ans;

    RInside R(argc, argv); 		        // create an embedded R instance 
    
    Rcpp::NumericMatrix M = createMatrix(mdim);	// create and fill a sample data Matrix 
    R["M"] = M;   	   		        // assign C++ matrix M to R's 'M' var

    std::string evalstr = "\
        cat('Running ls()\n'); print(ls()); 		       \
        cat('Showing M\n'); print(M);			       \
        cat('Showing colSums()\n'); Z <- colSums(M); print(Z); \
        Z";                     // returns Z

    ans = R.parseEval(evalstr);    		// eval the init string -- Z is now in ans
    						
    Rcpp::NumericVector v(ans);			// convert SEXP ans to a vector of doubles
    for (int i=0; i< v.size(); i++) {		// show the result
        std::cout << "In C++ element " << i << " is " << v[i] << std::endl;
    }
    exit(0);
}


