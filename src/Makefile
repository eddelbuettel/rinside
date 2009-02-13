## Simple Makefile
##
## RInside.cpp: Easier R embedding into C++
##
## Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed 
##
## TODO: 
##  proper configure for non-Debian file locations, 
##  allow RHOME to be set for non-default R etc

Sources	=		RInside.cpp MemBuf.cpp 

USERLIB	=		libRInside$(DYLIB_EXT) 
USERLIBST =		libRInside.a
USERDIR =		../inst/lib

CPPFLAGS =		-Wall -O3 
CXXFLAGS =		-I. -I/usr/share/R/include 
CXXFLAGS +=		-I/usr/local/lib/R/site-library/Rcpp/lib
LDFLAGS	=		-s
LDLIBS =		-L/usr/lib/R/lib -lR
CC =			g++

all: 			$(SHLIB) userLibrary 

#RInside.so :		RInside.cpp RInsideEnvVars.h RInsideAutoloads.h
#			$(CXX) -shared $(CPPFLAGS) $(CXXFLAGS) -o RInside.so $(Sources) $(LDFLAGS) $(LDLIBS)

RInsideEnvVars.h :	tools/RInsideEnvVars.r
			R --slave < tools/RInsideEnvVars.r > RInsideEnvVars.h

RInsideAutoloads.h :	tools/RInsideAutoloads.r
			R --slave < tools/RInsideAutoloads.r > RInsideAutoloads.h

userLibrary: 		$(USERLIB) $(USERLIBST)
			-@if test ! -e $(USERDIR)$(R_ARCH); then mkdir -p $(USERDIR)$(R_ARCH); fi
			cp $(USERLIB) $(USERDIR)$(R_ARCH)
			cp RInside.h MemBuf.h $(USERDIR)
			cp $(USERLIBST) $(USERDIR)$(R_ARCH)
			rm $(USERLIB) $(USERLIBST)

$(USERLIB): 		RInside.o MemBuf.o
			$(SHLIB_CXXLD) -o $(USERLIB) $^ $(SHLIB_CXXLDFLAGS) $(ALL_LIBS)
			@if test -e "/usr/bin/install_name_tool"; then \
				/usr/bin/install_name_tool -id $(R_PACKAGE_DIR)/lib$(R_ARCH)/$(USERLIB) $(USERLIB); fi

$(USERLIBST): 		RInside.o MemBuf.o
			$(AR) qc $(USERLIBST) RInside.o MemBuf.o
			@if test -n "$(RANLIB)"; then $(RANLIB) $(USERLIBST); fi

.PHONY: 		all clean userLibrary 

clean:
			rm -f $(OBJECTS) $(SHLIB) $(USERLIB) $(USERLIBST)

