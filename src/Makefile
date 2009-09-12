## Simple Makefile
##
## RInside.cpp: Easier R embedding into C++
##
## Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 
##
## TODO: 
##  proper configure for non-Debian file locations, 
##  allow RHOME to be set for non-default R etc

# these values and includes are need if you call 'make' rather than 'R CMD INSTALL'
#RHOME :=		$(shell R RHOME)
#R_SHARE_DIR := 	$(shell Rscript -e "cat(Sys.getenv('R_SHARE_DIR'))")
#include $(RHOME)/etc/Makeconf


SOURCES	:=		$(wildcard *.cpp)
OBJECTS	:= 		$(SOURCES:.cpp=.o)

HEADERS	:=		$(wildcard *.h)

USERLIB	=		libRInside$(DYLIB_EXT) 
USERLIBST =		libRInside.a
USERDIR =		../inst/lib

RCPPFLAGS :=		$(shell R CMD config --cppflags)
RLDFLAGS :=		$(shell R CMD config --ldflags)

RCPPINCL :=		$(shell Rscript -e 'Rcpp:::CxxFlags()')


CPPFLAGS =		-Wall -O3 
CXXFLAGS =		-I. $(RCPPFLAGS) $(RCPPINCL)
LDFLAGS	=		-s
LDLIBS =		$(RLDFLAGS)
CC =			g++


all: 			$(SHLIB) userLibrary 


.PHONY:			RInsideEnvVars.h
RInsideEnvVars.h :	tools/RInsideEnvVars.r
			R --slave < tools/RInsideEnvVars.r > RInsideEnvVars.h

.PHONY:			RInsideAutoloads.h
RInsideAutoloads.h :	tools/RInsideAutoloads.r
			R --slave < tools/RInsideAutoloads.r > RInsideAutoloads.h

userLibrary: 		$(USERLIB) $(USERLIBST)
			-@if test ! -e $(USERDIR)$(R_ARCH); then mkdir -p $(USERDIR)$(R_ARCH); fi
			cp $(USERLIB) $(USERDIR)$(R_ARCH)
			cp RInside.h MemBuf.h $(USERDIR)
			cp $(USERLIBST) $(USERDIR)$(R_ARCH)
			rm $(USERLIB) $(USERLIBST)

$(USERLIB): 		$(OBJECTS) $(HEADERS)
			$(SHLIB_CXXLD) -o $(USERLIB) $(OBJECTS) $(SHLIB_CXXLDFLAGS) $(ALL_LIBS)
			@if test -e "/usr/bin/install_name_tool"; then \
				/usr/bin/install_name_tool -id $(R_PACKAGE_DIR)/lib$(R_ARCH)/$(USERLIB) $(USERLIB); fi

$(USERLIBST): 		$(OBJECTS) $(HEADERS)
			$(AR) qc $(USERLIBST) $(OBJECTS)
			@if test -n "$(RANLIB)"; then $(RANLIB) $(USERLIBST); fi

.PHONY: 		all clean userLibrary 

clean:
			rm -f $(OBJECTS) $(SHLIB) $(USERLIB) $(USERLIBST)

