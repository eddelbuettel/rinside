// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Another simple example inspired by an r-devel mail by Abhijit Bera
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 

#include "RInside.h"                    // for the embedded R via RInside
#include <iomanip>

int main(int argc, char *argv[]) {

    try {
        RInside R(argc, argv);          // create an embedded R instance 
        SEXP ans;

        std::string txt = "suppressMessages(library(fPortfolio))";
        if (R.parseEvalQ(txt))          // load library, no return value
            throw std::runtime_error("R cannot evaluate '" + txt + "'");

        txt = "lppData <- 100 * LPP2005.RET[, 1:6]; "
	  "ewSpec <- portfolioSpec(); " 
	  "nAssets <- ncol(lppData); ";
        if (R.parseEval(txt, ans))      // prepare problem
            throw std::runtime_error("R cannot evaluate '" + txt + "'");
	
	const double dvec[6] = { 0.1, 0.1, 0.1, 0.1, 0.3, 0.3 }; // choose any weights you want
	const std::vector<double> w(dvec, &dvec[6]);

	R.assign( w, "weightsvec");	// assign STL vector to R's 'weightsvec' variable

	txt = "setWeights(ewSpec) <- weightsvec";
        if (R.parseEvalQ(txt))		// evaluate assignment
            throw std::runtime_error("R cannot evaluate '" + txt + "'");

	txt = "ewPortfolio <- feasiblePortfolio(data = lppData, spec = ewSpec, constraints = \"LongOnly\"); "
	  "print(ewPortfolio); "
	  "vec <- getCovRiskBudgets(ewPortfolio@portfolio)";
        if (R.parseEval(txt, ans))      // assign covRiskBudget weights to ans
            throw std::runtime_error("R cannot evaluate '" + txt + "'");
	RcppVector<double> V(ans);      // convert SEXP variable to an RcppVector
  
	R.parseEval("names(vec)", ans);	// assign columns names to ans
	RcppStringVector names(ans);   

	for (int i=0; i<names.size(); i++) {
	  std::cout << std::setw(16) << names(i) << "\t"
		    << std::setw(11) << V(i) << "\n";
        }
        
    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    exit(0);
}

