## Simple Makefile
##
## RInside.cpp: Easier R embedding into C++
##
## Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 
##
## Assumptions:  
##    This Makefile is inside the src/ directory of the RInside
##    package and should only be called by R via 'R CMD INSTALL'
##    'R CMD build', 'R CMD check' etc.  Thus we can assume R_HOME
##    below which should be all that is needed to support several
##    R installations at the same time.

SOURCES	:= 		$(wildcard *.cpp)
OBJECTS	:= 		$(SOURCES:.cpp=.o)


HEADERS	:= 		RInsideEnvVars.h RInsideAutoloads.h

USERLIB	= 		libRInside$(DYLIB_EXT) 
USERLIBST =		libRInside.a
USERDIR = 		../inst/lib

RCPPFLAGS := 		$(shell $(R_HOME)/bin/R CMD config --cppflags)
RLDFLAGS := 		$(shell $(R_HOME)/bin/R CMD config --ldflags)

RCPPINCL := 		$(shell echo 'Rcpp:::CxxFlags()' | $(R_HOME)/bin/R --vanilla --slave)

CXX := 			$(shell $(R_HOME)/bin/R CMD config CXX)
CPPFLAGS := 		-Wall $(shell $(R_HOME)/bin/R CMD config CPPFLAGS)
CXXFLAGS := 		-I. $(RCPPFLAGS) $(RCPPINCL) $(shell $(R_HOME)/bin/R CMD config CXXFLAGS)
LDFLAGS	= 		-s
LDLIBS = 		$(RLDFLAGS)


all : 			$(SHLIB) userLibrary 

.PHONY :		RInsideEnvVars.h
RInsideEnvVars.h :	tools/RInsideEnvVars.r
			$(R_HOME)/bin/R --vanilla --slave < tools/RInsideEnvVars.r > RInsideEnvVars.h

.PHONY :		RInsideAutoloads.h
RInsideAutoloads.h :	tools/RInsideAutoloads.r
			$(R_HOME)/bin/R --vanilla --slave < tools/RInsideAutoloads.r > RInsideAutoloads.h

userLibrary : 		$(USERLIB) $(USERLIBST)
			-@if test ! -e $(USERDIR)$(R_ARCH); then mkdir -p $(USERDIR)$(R_ARCH); fi
			cp $(USERLIB) $(USERDIR)$(R_ARCH)
			cp RInside.h MemBuf.h $(USERDIR)$(R_ARCH)
			cp $(USERLIBST) $(USERDIR)$(R_ARCH)
			rm $(USERLIB) $(USERLIBST)

$(USERLIB) : 		$(HEADERS) $(OBJECTS) 
			$(SHLIB_CXXLD) -o $(USERLIB) $(OBJECTS) $(SHLIB_CXXLDFLAGS) $(ALL_LIBS)
			@if test -e "/usr/bin/install_name_tool"; then \
				/usr/bin/install_name_tool -id $(R_PACKAGE_DIR)/lib$(R_ARCH)/$(USERLIB) $(USERLIB); fi

$(USERLIBST) : 		$(HEADERS) $(OBJECTS) 
			$(AR) qc $(USERLIBST) $(OBJECTS)
			@if test -n "$(RANLIB)"; then $(RANLIB) $(USERLIBST); fi

.PHONY : 		all clean userLibrary 

clean:
			rm -f $(OBJECTS) $(SHLIB) $(USERLIB) $(USERLIBST) $(HEADERS)

