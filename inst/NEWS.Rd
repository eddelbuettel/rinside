\name{NEWS}
\title{News for Package 'RInside'}
\newcommand{\cpkg}{\href{http://CRAN.R-project.org/package=#1}{\pkg{#1}}}

\section{Changes in RInside version 0.2.8 (2012-xx-yy)}{
  \itemize{
    \item Added CMake build support for armadillo and eigen examples,
    once again kindly contributed by Peter Aberline
   }
}
\section{Changes in RInside version 0.2.7 (2012-08-12)}{
  \itemize{
    \item New fifth examples subdirectory 'armadillo' with two new
    examples showing how to combine \cpkg{RInside} with \cpkg{RcppArmadillo}
    \item New sixth examples subdirectory 'eigen' with two new examples
    showing how to combine \cpkg{RInside} with \cpkg{RcppEigen} 
    \item Prettified the Wt example 'web application' with CSS use, also added
    and XML file with simple headers and description text
    \item New example rinside_sample12 motivated by StackOverflow
    question on using \code{sample()} from C
    \item Added CMake build support on Windows for the examples
  }
}
\section{Changes in RInside version 0.2.6 (2012-01-11)}{
  \itemize{
    \item Correct Windows initialization by not using Rprintf in internal
    console writer, with thanks to both James Bates and John Brzustowski
    \item Update RNG seeding (used by tmpnam et al) to same scheme used by
    R since 2.14.0: blending both millisecond time and process id
    \item Added CMake build support for all four example directories as kindly 
    provided by Peter Aberline; this helps when writing RInside code
    inside of IDEs such as Eclipse, KDevelop or Code::Blocks
    \item Small update to standard examples Makefile for Windows permitting 
    to explicitly set i386 or x64 as a build architecture
  }
}
\section{Changes in RInside version 0.2.5 (2011-12-07)}{
  \itemize{
    \item Applied (somewhat simplified) patch by James Bates which restores
    RInside to working on Windows -- with a big Thank You! to James for
    fixing a long-standing bug we inadvertendly introduced right after
    0.2.0 almost two years ago
    \item New example embedding R inside a Wt (aka Webtoolkit, pronounced 
    'witty') application, mirroring the previous Qt application
    \item Qt example qtdensity now uses the new svg() device in base R; removed
    test for cairoDevice package as well as fallback png code
    \item Very minor fix to qmake.pro file for Qt app correcting link order
  }
}
\section{Changes in RInside version 0.2.4 (2011-04-24)}{
  \itemize{
    \item Minor code cleanups in initialization code 
    \item New example embedding R inside a Qt application, along with pro file
    for Qt's qmake providing a complete simple C++ GUI application
    \item New examples rinside_sample\{10,11\} based on questions on the
    r-help and r-devel mailing list
    \item Some improvements and simplifications throughout examples/standard 
    as well as examples/mpi/
    \item Added this NEWS files -- with entries below summarised from ChangeLog
    and the corresponding blog posts
  }
}
\section{Changes in RInside version 0.2.3 (2010-08-06)}{
  \itemize{
    \item New example rinside_sample9 on how to expose C++ to embedded R
    \item New example rinside_module_sample0 to show module access from RInside
    \item Simplified rinside_sample3 and rinside_sample4
    \item Some code cleanup to help Solaris builds
    \item Implicit use of new Proxy class with operator T(), see rinside_sample8
  }
}
\section{Changes in RInside version 0.2.2 (2010-03-22)}{
  \itemize{
    \item New operator[](string) lets RInside act as proxy to R's global
    environment so that we can R["x"] = 10 to assign; all the actual
    work is done by Rcpp::Environment
    \item No longer ship doxygen-generated docs in build
    \item Use std::string for all arguments inside throw() to help Windows build
    \item Default to static linking on OS X and Windows just like Rcpp does
    \item parseEval() now returns SEXP and has just a string argument for more
    functional use; it and void sibbling parseEvalQ() now throw exections
    \item rinside_sample\{2,4,5\} updated accordingly
    \item Two new 'R inside an MPI app' examples contributed by Jianping Hua
    \item Also added two C++ variants of the C examples for RInside and MPI
    \item rinside_sample8 updated with parseEval changes   
    \item Internal MemBuf class simplified via STL std::string
    \item Autoload simplied via the new Rcpp API
    \item Added default constructor for RInside
    \item Retire assign(vector<vector<double> >) via template specialisation
    \item Include Rcpp.h; switch to Rf_ prefixed R API to avoid Redefine macros
    \item Windows version currently segfaults on startup
  }
}
\section{Changes in RInside version 0.2.1 (2010-01-06)}{
  \itemize{
    \item Startup now defaults to FALSE, no longer call Rf_KillAllDevices
    \item Some minor build and code fixes for Windows
  }
}
\section{Changes in RInside version 0.2.0 (2009-12-20)}{
  \itemize{
    \item Initial Windows support, with thanks to Richard Holbrey for both the
    initial push and a setenv() implementation
    \item Added Makefile.win for build with the MinGW toolchain to src/ and examples/
    \item Some improvements to destructor per example in Writing R Extensions
    \item New rinside_sample5 based on r-devel post
  }
}
\section{Changes in RInside version 0.1.1 (2009-02-19)}{
  \itemize{
    \item The examples/ Makefile now sets $R_HOME via 'R RHOME', and also employs
    $R_ARCH for arch-dependent headers -- with thanks for Jeff, Jan and Simon
    \item Added THANKS file to give recognition to those who helped RInside along
    \item Added rinside_sample4 as another example based on an r-devel question
  }
}
\section{Changes in RInside version 0.1.0 (2009-02-19)}{
  \itemize{
    \item Initial CRAN release
    \item Improved build process
    \item Added doxygen generated documentation
    \item Added two more example
  }
}
\section{Changes in RInside version 0.0.1 (2009-07-19)}{
  \itemize{
    \item Corrected error in memory buffer class with thanks to Miguel Lechón for
    a finding the issue and sending a patch
    \item Added two regression test examples to demonstrate bug and fix
    \item Minor code cleanups
    \item Initial version in SVN at R-Forge
  }
}