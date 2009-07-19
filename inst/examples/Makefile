
## Simple Makefile
##
## TODO: 
##  proper configure for non-Debian file locations, 
##  allow RHOME to be set for non-default R etc

CPPFLAGS		= -Wall -O3 
CXXFLAGS		= -I. -I/usr/share/R/include 
CXXFLAGS		+= -I/usr/lib/R/site-library/Rcpp/lib
CXXFLAGS		+= -I/usr/local/lib/R/site-library/RInside/lib
LDFLAGS			= -s
LDLIBS			= -L/usr/lib/R/lib -lR
LDLIBS			+= -L/usr/lib/R/site-library/Rcpp/lib -lRcpp -Wl,-rpath,/usr/lib/R/site-library/Rcpp/lib
LDLIBS			+= -L/usr/local/lib/R/site-library/RInside/lib -lRInside -Wl,-rpath,/usr/local/lib/R/site-library/RInside/lib
CC			= g++

sources 		:= $(wildcard *.cpp)
programs 		:= $(sources:.cpp=)

all:			$(programs)
#all :			rinside_sample0 rinside_sample1

check:
			@echo Sources are $(sources)
			@echo Programs are $(programs)

clean:
			rm -vf $(programs)

#rinside_sample0 :	rinside_sample0.cpp
#			$(CC) -o $@ $< $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

#rinside_sample1 :	rinside_sample1.cpp
#			$(CC) -o $@ $< $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

#rinside_test0 :		rinside_test0.cpp
#			$(CC) -o $@ $< $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

#rinside_test1 :		rinside_test1.cpp
#			$(CC) -o $@ $< $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
