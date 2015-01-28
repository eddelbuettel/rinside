// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// cf https://github.com/RcppCore/Rcpp/issues/178

#include <RInside.h>                    // for the embedded R via RInside

int main(int argc, char *argv[]) {

    try {

        RInside R(argc, argv);          // create an embedded R instance 

        std::vector<double> data(15);
        for_each(data.begin(), data.end(), [](double &val) { val = rand(); });
        //for (int i=0; i<15; i++) data[i] = rand();
        R["data"] = data;

        R.parseEvalQ("print(summary(data))");
        
    } catch(std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    exit(0);
}

