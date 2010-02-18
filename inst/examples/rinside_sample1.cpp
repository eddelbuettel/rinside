// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8; -*-
//
// Simple example with data in C++ that is passed to R, processed and a result is extracted
//
// Copyright (C) 2009 Dirk Eddelbuettel 
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois
//
// GPL'ed 

#include "RInside.h"            // for the embedded R via RInside

std::vector< std::vector< double > > 
createMatrix(const int n) {     // simple STL matrix content generator
    std::vector< std::vector< double > > mat;
    for (int i=0; i<n; i++) {
        std::vector<double> row;
        for (int j=0; j<n; j++) {
            row.push_back((i*10+j)); 
        }
        mat.push_back(row);
    }
    return(mat);
}

int main(int argc, char *argv[]) {
    const int mdim = 4;
    SEXP ans;

    RInside R(argc, argv); 		        // create an embedded R instance 
    
    // create and fill a sample data Matrix 
    std::vector< std::vector< double > > myMatrix = createMatrix(mdim);
    
    R["M"] = myMatrix;      		        // assign STL matrix to R's 'M' var

    std::string evalstr = "\
        cat('Running ls()\n'); print(ls()); 		       \
        cat('Showing M\n'); print(M);			       \
        cat('Showing colSums()\n'); Z <- colSums(M); print(Z); \
        Z";                     // returns Z

    R.parseEval(evalstr, ans);                  // eval the init string -- Z is now in ans

    						// convert SEXP ans to a vector of doubles
    std::vector<double> v = Rcpp::as <std::vector<double> >(ans);

    for (unsigned int i=0; i< v.size(); i++) {	// show the result
        std::cout << "In C++ element " << i << " is " << v[i] << std::endl;
    }
    exit(0);
}


