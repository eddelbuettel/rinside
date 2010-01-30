// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8; -*-
//
// Simple example with data in C++ that is passed to R, processed and a result is extracted
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 

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
    std::string evalstr = "\
        cat('Running ls()\n'); print(ls()); \
        cat('Showing M\n'); print(M); \
        cat('Showing colSums()\n'); Z <- colSums(M); print(Z); \
        Z";                     // returns Z

    RInside R(argc, argv);
    SEXP ans;
    
    // create and fill a sample data Matrix 
    std::vector< std::vector< double > > myMatrix = createMatrix(mdim);
    
    R.assign( myMatrix, "M");                   // assign STL matrix to R's 'M' var
    R.parseEval(evalstr, ans);                  // eval the init string -- Z is now in ans

    RcppVector<double> vec(ans);                // now vec contains Z via ans
    std::vector<double> v = vec.stlVector();    // convert RcppVector to STL vector
    // We could also do the assignment in one line:
    //    vector<double> v = RcppVector<double>(ans).stlVector();

    // show the result
    for (unsigned int i=0; i< v.size(); i++) {
        std::cout << "In C++ element " << i << " is " << v[i] << std::endl;
    }
    exit(0);
}


