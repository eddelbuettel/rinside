// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: R/C++ interface class library -- Easier R embedding into C++
//
// Copyright (C) 2009        Dirk Eddelbuettel
// Copyright (C) 2010        Dirk Eddelbuettel and Romain Francois
//
// This file is part of RInside.
//
// RInside is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// RInside is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RInside.  If not, see <http://www.gnu.org/licenses/>.

#include "RInside.h"
#include <sys/time.h>		// gettimeofday

bool verbose = false;
const char *programName = "RInside";

#ifdef WIN32
// on Windows, we need to provide setenv which is in the file setenv.c here
#include "setenv.c"
extern int optind;
#endif

RInside::~RInside() {		// now empty as MemBuf is internal
    logTxt("RInside::dtor BEGIN", verbose);
    R_dot_Last();
    R_RunExitFinalizers();
    R_CleanTempDir();
    //Rf_KillAllDevices();
    //#ifndef WIN32
    //fpu_setup(FALSE);
    //#endif
    Rf_endEmbeddedR(0);
    logTxt("RInside::dtor END", verbose);
}

RInside::RInside() {
	initialize( 0, 0 );
}

RInside::RInside(const int argc, const char* const argv[]) {
	initialize( argc, argv ); 
}

/* TODO: use a vector<string> would make all this a bit more readable */
void RInside::initialize(const int argc, const char* const argv[]){
    logTxt("RInside::ctor BEGIN", verbose);

    verbose_m = false; 		// Default is false

    // generated as littler.h via from svn/littler/littler.R
    #include "RInsideEnvVars.h"

    for (int i = 0; R_VARS[i] != NULL; i+= 2) {
	if (getenv(R_VARS[i]) == NULL) { // if env variable is not yet set
	    if (setenv(R_VARS[i],R_VARS[i+1],1) != 0){
		perror("ERROR: couldn't set/replace an R environment variable");
		exit(1);
	    }
	}
    }

    #ifndef WIN32
    R_SignalHandlers = 0;    		// Don't let R set up its own signal handlers
    #endif

    #ifdef CSTACK_DEFNS
    R_CStackLimit = (uintptr_t)-1;	// Don't do any stack checking, see R Exts, '8.1.5 Threading issues' 
    #endif

    init_tempdir();

    const char *R_argv[] = {(char*)programName, "--gui=none", "--no-save", "--no-readline", "--silent", "", ""};
    const char *R_argv_opt[] = {"--vanilla", "--slave"};
    int R_argc = (sizeof(R_argv) - sizeof(R_argv_opt) ) / sizeof(R_argv[0]);
    Rf_initEmbeddedR(R_argc, (char**)R_argv);

    R_ReplDLLinit(); 		// this is to populate the repl console buffers 

    structRstart Rst;
    R_DefParams(&Rst);
    Rst.R_Interactive = (Rboolean) FALSE;	// sets interactive() to eval to false 
    R_SetParams(&Rst);

    //ptr_R_CleanUp = littler_CleanUp; --- we do that in the destructor
    global_env = R_GlobalEnv ;
    
    autoloads();    		// Force all default package to be dynamically required */

    if ((argc - optind) > 1){    	// for argv vector in Global Env */
	Rcpp::CharacterVector s_argv( argv+(1+optind), argv+argc );
	assign(s_argv, "argv");
    } else {
	assign(R_NilValue, "argv") ;
    }
  
    init_rand();    			// for tempfile() to work correctly */
    logTxt("RInside::ctor END", verbose);

    
}

void RInside::init_tempdir(void) {
    const char *tmp;
    // FIXME:  if per-session temp directory is used (as R does) then return
    tmp = getenv("TMPDIR");
    if (tmp == NULL) {
	tmp = getenv("TMP");
	if (tmp == NULL) { 
	    tmp = getenv("TEMP");
	    if (tmp == NULL) 
		tmp = "/tmp";
	    }
    }
    R_TempDir = (char*) tmp;
    if (setenv("R_SESSION_TMPDIR",tmp,1) != 0){
	perror("Fatal Error: couldn't set/replace R_SESSION_TMPDIR!");
	exit(1);
    }
}

void RInside::init_rand(void) {	/* set seed for tempfile()  */
    unsigned int seed;
    struct timeval tv;
    gettimeofday (&tv, NULL);
    // changed uint64_t to unsigned int. Need to figure out why BDR used that instead. 
    seed = ((unsigned int) tv.tv_usec << 16) ^ tv.tv_sec;
    srand(seed);
}

void RInside::autoloads() {

    #include "RInsideAutoloads.h"
 
    /* Autoload default packages and names from autoloads.h
     *
     * This function behaves in almost every way like
     * R's autoload:
     * function (name, package, reset = FALSE, ...)
     * {
     *     if (!reset && exists(name, envir = .GlobalEnv, inherits = FALSE))
     *        stop("an object with that name already exists")
     *     m <- match.call()
     *     m[[1]] <- as.name("list")
     *     newcall <- eval(m, parent.frame())
     *     newcall <- as.call(c(as.name("autoloader"), newcall))
     *     newcall$reset <- NULL
     *     if (is.na(match(package, .Autoloaded)))
     *        assign(".Autoloaded", c(package, .Autoloaded), env = .AutoloadEnv)
     *     do.call("delayedAssign", list(name, newcall, .GlobalEnv,
     *                                                         .AutoloadEnv))
     *     invisible()
     * }
     *
     * What's missing is the updating of the string vector .Autoloaded with 
     * the list of packages, which by my code analysis is useless and only 
     * for informational purposes.
     *
     */
    
    /* we build the call : 
    
        delayedAssign( NAME, 
        	autoloader( name = NAME, package = PACKAGE), 
        	.GlobalEnv, 
        	.AutoloadEnv )
        	
        where : 
        - PACKAGE is updated in a loop
        - NAME is updated in a loop
        
    */
     
    int i,j, idx=0, nobj ;
    Rcpp::Language delayed_assign_call( 
    	    Rcpp::Function("delayedAssign"), 
    	    R_NilValue,     /* arg1: assigned in loop */
    	    R_NilValue,     /* arg2: assigned in loop */
    	    global_env,
    	    global_env.find(".AutoloadEnv")
    	    ) ;
    Rcpp::Language::Proxy delayed_assign_name  = delayed_assign_call[1];

    Rcpp::Language autoloader_call( 
    	    Rcpp::Function("autoloader"),
    	    Rcpp::Named( "name", R_NilValue) ,  /* arg1 : assigned in loop */
    	    Rcpp::Named( "package", R_NilValue) /* arg2 : assigned in loop */
    	    );
    Rcpp::Language::Proxy autoloader_name = autoloader_call[1];
    Rcpp::Language::Proxy autoloader_pack = autoloader_call[2];
    delayed_assign_call[2] = autoloader_call ;
    
    try{
    	for( i=0; i<packc; i++){
    		
    		/* set the 'package' argument of the autoloader call */
    		autoloader_pack = pack[i] ;
		
    		nobj = packobjc[i] ; 
    		for (j = 0; j < nobj ; j++){
		    
		    /* set the 'name' argument of the autoloader call */ 
		    autoloader_name = packobj[idx+j] ;
		   
		    /* Set the 'name' argument of the delayedAssign call */
		    delayed_assign_name = packobj[idx+j] ;
		    
		    /* evaluate the call */
		    delayed_assign_call.eval() ;
		    
		}
		
		idx += packobjc[i] ;
		
    	}
    } catch( std::exception& ex){
    	    fprintf(stderr,"%s: Error calling delayedAssign:\n %s", 
		programName, ex.what() );
	    exit(1);	    
    }
}

int RInside::parseEval(const std::string & line, SEXP & ans) {
    ParseStatus status;
    SEXP cmdSexp, cmdexpr = R_NilValue;
    int i, errorOccurred;

    mb_m.add((char*)line.c_str());
    
    PROTECT(cmdSexp = Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(cmdSexp, 0, Rf_mkChar(mb_m.getBufPtr()));

    cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));

    switch (status){
    case PARSE_OK:
	/* Loop is needed here as EXPSEXP might be of length > 1 */
	for(i = 0; i < Rf_length(cmdexpr); i++){
	    ans = R_tryEval(VECTOR_ELT(cmdexpr, i),NULL,&errorOccurred);
	    if (errorOccurred) {
		fprintf(stderr, "%s: Error in evaluating R code (%d)\n", programName, status);
		UNPROTECT(2);
		return 1;
	    }
	    if (verbose_m) {
		Rf_PrintValue(ans);
	    }
	}
	mb_m.rewind();
	break;
    case PARSE_INCOMPLETE:
	/* need to read another line */
	break;
    case PARSE_NULL:
	fprintf(stderr, "%s: ParseStatus is null (%d)\n", programName, status);
	UNPROTECT(2);
	return 1;
	break;
    case PARSE_ERROR:
	fprintf(stderr,"Parse Error: \"%s\"\n", line.c_str());
	UNPROTECT(2);
	return 1;
	break;
    case PARSE_EOF:
	fprintf(stderr, "%s: ParseStatus is eof (%d)\n", programName, status);
	break;
    default:
	fprintf(stderr, "%s: ParseStatus is not documented %d\n", programName, status);
	UNPROTECT(2);
	return 1;
	break;
    }
    UNPROTECT(2);
    return 0;
}

int RInside::parseEvalQ(const std::string & line) {
    SEXP ans;
    int rc = parseEval(line, ans);
    return rc;
}

SEXP RInside::parseEval(const std::string & line) {
    SEXP ans;
    int rc = parseEval(line, ans);
    return ans;
}

Rcpp::Environment::Binding RInside::operator[]( const std::string& name ){
	return global_env[name]; 
}

// specializations of Rcpp wrap template
// this overwrites the default behaviour in Rcpp that would 
// wrap vector<vector<double>> as lists of numeric vectors
//
// RInside uses vector<vector<double>> as matrices

namespace Rcpp{

template<> SEXP wrap(const std::vector< std::vector< double > > & v) {
    int nx = v.size();
    int ny = v[0].size();
    SEXP sexpmat = PROTECT(Rf_allocMatrix(REALSXP, nx, ny));
    double* p = REAL(sexpmat) ;
    for(int i = 0; i < nx; i++) {
    	for(int j = 0; j < ny; j++) {
	    p[i + nx*j] = v[i][j];
	}
    }
    UNPROTECT(1);
    return sexpmat ;
}

template<> SEXP wrap(const std::vector< std::vector< int > > & v) {
    int nx = v.size();
    int ny = v[0].size();
    SEXP sexpmat = PROTECT(Rf_allocMatrix(INTSXP, nx, ny));
    int *p = INTEGER(sexpmat) ; /* do this just once */
    for(int i = 0; i < nx; i++) {
	for(int j = 0; j < ny; j++) {
	    p[i + nx*j] = v[i][j];
	}
    }
    UNPROTECT(1);
    return sexpmat ;
}

}
