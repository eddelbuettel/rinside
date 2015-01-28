////////// dataptr_crash.cpp //////

#include <RInside.h>
#include <Rcpp.h>
#include <boost/shared_ptr.hpp>

static boost::shared_ptr<RInside> Rinst;
 
int main(int argc, char *argv[]) {
    Rinst = boost::shared_ptr<RInside>(new RInside(argc, argv));
    const int N = 173;          // made const for -Wall ....
    double v[N];
    std::fill(v, v+N, 0.0);
    Rcpp::NumericVector x(v, v+N);
    std::cout << "All good\n";
    return 0;
}
