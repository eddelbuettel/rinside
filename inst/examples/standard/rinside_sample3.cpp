// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8; -*-
//
// Simple example for using lm() using the example from help(swiss)
//
// Copyright (C) 2009 Dirk Eddelbuettel 
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois

#include "RInside.h"                    // for the embedded R via RInside
#include <iomanip>

int main(int argc, char *argv[]) {

    RInside R(argc, argv);              // create an embedded R instance 
    SEXP ans;

    std::string txt =                   // load library, run regression, create summary
        "suppressMessages(require(stats));"     
        "swisssum <- summary(lm(Fertility ~ . , data = swiss));" 
        "print(swisssum)";             
    R.parseEvalQ(txt);                  // eval command, no return

    txt = "swcoef <- coef(swisssum)";
    R.parseEval(txt, ans);              // matrix swisscoef now in SEXP ans
    RcppMatrix<double> M(ans);          // convert SEXP variable to an RcppMatrix
  
    R.parseEval("colnames(swcoef)",ans);// assign columns names to ans
    RcppStringVector cnames(ans);       // and into string vector cnames
    R.parseEval("rownames(swcoef)",ans);// assign columns names to ans
    RcppStringVector rnames(ans);       // and into string vector cnames

    std::cout << "\t\t\t";
    for (int i=0; i<cnames.size(); i++) {
        std::cout << std::setw(11) << cnames(i) << "\t";
    }
    std::cout << std::endl;

    for (int i=0; i<rnames.size(); i++) {
        std::cout << std::setw(16) << rnames(i) << "\t";
        for (int j=0; j<cnames.size(); j++) {
            std::cout << std::setw(11) << M(i,j) << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    exit(0);
}

