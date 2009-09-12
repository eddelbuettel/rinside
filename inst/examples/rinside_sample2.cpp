// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; -*-
//
// Simple example for the repeated r-devel mails by Abhijit Bera
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 

#include "RInside.h"			// for the embedded R via RInside
#include "Rcpp.h"			// for the R / Cpp interface used for transfer

int main(int argc, char *argv[]) {

    RInside R(argc, argv);		// create an embedded R instance 
    SEXP ans;

    std::string txt = "suppressMessages(library(fPortfolio))";
    R.parseEvalQ(txt); 			// load library, no return valueassign matrix M to SEXP variable ans

    txt = "M <- as.matrix(SWX.RET); print(head(M)); M";
    R.parseEval(txt, ans); 		// assign matrix M to SEXP variable ans
    RcppMatrix<double> M(ans);		// convert SEXP variable to an RcppMatrix
  
    std::cout << "M has " << M.getDim1() << " rows and " << M.getDim2() << " cols" << std::endl;
  
    R.parseEval("colnames(M)", ans); 	// assign columns names of M to ans
    RcppStringVector cnames(ans);	// and into string vector cnames
  

    for (int i=0; i<M.getDim2(); i++) {
	std::cout << "Column " << cnames(i) << " in row 42 has " << M(42,i) << std::endl;
    }

    exit(0);
}

