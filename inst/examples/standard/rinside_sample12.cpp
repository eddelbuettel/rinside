// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Simple example motivated by post(s) from Nandan Amar to to rcpp-devel
//
// Copyright (C) 2011  Dirk Eddelbuettel and Romain Francois

#include <RInside.h>                    // for the embedded R via RInside

int main(int argc, char *argv[]) {

    RInside R(argc, argv);    		// create an embedded R instance

    std::string cmd = 			// evaluate an R expression with curve()
	"tmpf <- tempfile('curve'); "	// get a temp. file
	"png(tmpf); "			// open a PNG device
	"curve(x^2, -10, 10, 200); "	// plot
	"dev.off();"			// properly close device
	"tmpf";				// filename as last value in expression
    std::string tfile = R.parseEval(cmd);// we get the last assignment, here the filename

    std::cout << "Could now use plot in " << tfile << std::endl;
    unlink(tfile.c_str()); 		// cleaning up
    
    // alternatively, by explicitly opening a display we can plot to screen
    cmd = "x11(); curve(x^2, -10, 10, 200); Sys.sleep(30);";
    R.parseEvalQ(cmd);			// parseEvalQ evluates without assignment
    
    exit(0);
}
