
#include <Rcpp.h>

// [[Rcpp::export]]
void showCompiler() {
#if defined(__DATE__)
    const char *date = __DATE__;
#else
    const char *date = "<unknown>";
#endif
#if defined(__VERSION__)
    const char *ver = __VERSION__;
#else
    const char *ver = "<unknown>";
#endif
    Rcpp::Rcout << "Compiled on " << date << " by compiler version " << ver << std::endl;
}
