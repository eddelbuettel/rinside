## RInside [![Build Status](https://travis-ci.org/eddelbuettel/rinside.svg)](https://travis-ci.org/eddelbuettel/rinside) [![License](http://img.shields.io/badge/license-GPL%20%28%3E=%202%29-brightgreen.svg?style=flat)](http://www.gnu.org/licenses/gpl-2.0.html) [![CRAN](http://www.r-pkg.org/badges/version/RInside)](https://cran.r-project.org/package=RInside) [![Downloads](http://cranlogs.r-pkg.org/badges/RInside?color=brightgreen)](https://cran.r-project.org/package=RInside)

Easy embedding of R inside C++

### About

The RInside package provides a few classes for seamless embedding of [R](https://www.r-project.org) inside of
C++ applications by relying on [Rcpp](http://www.rcpp.org).

### Examples

Provided with the package itself are eight subdirectories with examples: from
more than a dozen basic command-line examples (in directory `standard`) to
graphical user-interfaces (using both [Qt](http://www.qt.io) and
[Wt](http://www.webtoolkit.eu/wt)), linear algebra with
[Armadillo](http://arma.sf.net) and
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page), parallel
computing with MPI to a sandboxed server.

The simplest example (modulo its header) is [examples/standard/rinside_sample0.cpp](inst/examples/standard/rinside_sample0.cpp)

```c++
#include <RInside.h>                    // for the embedded R via RInside

int main(int argc, char *argv[]) {

    RInside R(argc, argv);              // create an embedded R instance 

    R["txt"] = "Hello, world!\n";	    // assign a char* (string) to 'txt'

    R.parseEvalQ("cat(txt)");           // eval the init string, ignoring any returns

    exit(0);
}
```
The [Qt example directory](https://github.com/eddelbuettel/rinside/tree/master/inst/examples/qt) produces 
this application for showing how to use R (to estimate densities) inside a C++ executable (providing the GUI):

![](https://github.com/eddelbuettel/rinside/blob/master/local/qtdensitySVG.png)

The code is portable across operating systems.  Similar, the 
[Wt example directory](https://github.com/eddelbuettel/rinside/tree/master/inst/examples/qt)
contains this C++-based web application doing the same:

![](https://github.com/eddelbuettel/rinside/blob/master/local/wtdensity.png)


### See Also

The [RInside](http://dirk.eddelbuettel.com/code/rinside.html) web page has
some more details.

### Authors

Dirk Eddelbuettel and Romain Francois

### License

GPL (>= 2)
