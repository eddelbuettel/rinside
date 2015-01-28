
#include <RInside.h>

int main(int argc, char *argv[])
{

  RInside R(argc, argv);

  R["txt"] = "Hello, world!\n";
  Rcpp::DataFrame pf;    //         <-------- Runs fine if I comment out this line.
  R.parseEvalQ("cat(txt)");

  exit(0);
}
