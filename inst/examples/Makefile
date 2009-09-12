
## Simple Makefile
##
## TODO: 
##  proper configure for non-Debian file locations,   [ Done ]
##  allow RHOME to be set for non-default R etc

## include headers and libraries for R 
RCPPFLAGS :=		$(shell R CMD config --cppflags)
RLDFLAGS :=		$(shell R CMD config --ldflags)

## include headers and libraries for Rcpp interface classes
RCPPINCL :=		$(shell Rscript -e 'Rcpp:::CxxFlags()')
RCPPLIBS :=		$(shell Rscript -e 'Rcpp:::LdFlags()')

## include headers and libraries for RInside embedding classes
RINSIDEINCL :=		$(shell Rscript -e 'RInside:::CxxFlags()')
RINSIDELIBS :=		$(shell Rscript -e 'RInside:::LdFlags()')


CPPFLAGS		= -Wall -O3 
CXXFLAGS		= $(RCPPFLAGS) $(RCPPINCL) $(RINSIDEINCL)
LDFLAGS			= -s
LDLIBS			= $(RLDFLAGS) $(RCPPLIBS) $(RINSIDELIBS)
CC			= g++

sources 		:= $(wildcard *.cpp)
programs 		:= $(sources:.cpp=)

all:			$(programs)

clean:
			rm -vf $(programs)

