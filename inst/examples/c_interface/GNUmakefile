# Basic R extensions info
R_HOME := $(shell R RHOME)
RLDFLAGS := $(shell $(R_HOME)/bin/R CMD config --ldflags)

# Header files required by Rcpp
RCPPFLAGS := $(shell $(R_HOME)/bin/R CMD config --cppflags)

# Location of RInsideC.h
RINSIDECINCL := $(shell echo 'RInside:::CFlags()' | $(R_HOME)/bin/R --vanilla --slave)

# Location of RInsideC library
RINSIDECLIBS := $(shell echo 'RInside:::LdFlags()'  | $(R_HOME)/bin/R --vanilla --slave)

# C compiler
CC := $(shell $(R_HOME)/bin/R CMD config CC)

FLAGS := $(RLDFLAGS) $(RCPPFLAGS) $(RINSIDECLIBS) $(RINSIDECINCL)

hello:
	$(CC) hello.c $(FLAGS) -o hello
	./hello

pass:
	$(CC) passdata.c $(FLAGS) -o passdata
	./passdata
