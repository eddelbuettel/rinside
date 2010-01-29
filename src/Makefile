## Simple Makefile
##
## R/C++ interface class library -- Easier R embedding into C++
##
## Copyright (C) 2009 - 2010 Dirk Eddelbuettel
##
## This file is part of RInside.
##
## RInside is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 2 of the License, or
## (at your option) any later version.
##
## RInside is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with RInside.  If not, see <http://www.gnu.org/licenses/>.
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
CPPFLAGS := 		-Wall $(RCPPFLAGS) $(RCPPINCL) 
CXXFLAGS := 		-I. $(RCPPFLAGS) $(shell $(R_HOME)/bin/R CMD config CXXFLAGS)
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

