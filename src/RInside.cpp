// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: R/C++ interface class library -- Easier R embedding into C++
//
// Copyright (C) 2009 - 2010 Dirk Eddelbuettel
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

bool verbose = true;
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

RInside::RInside(const int argc, const char* const argv[]) {
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

    autoloads();    		// Force all default package to be dynamically required */

    SEXP s_argv = R_NilValue;
    if ((argc - optind) > 1){    	// for argv vector in Global Env */
	int nargv = argc - optind - 1;	// Build string vector 
	PROTECT(s_argv = allocVector(STRSXP,nargv));
	for (int i = 0; i <nargv; i++){
	    STRING_PTR(s_argv)[i] = mkChar(argv[i+1+optind]);
	}
	UNPROTECT(1);
    
	setVar(install("argv"),s_argv,R_GlobalEnv);
    } else {
	setVar(install("argv"),R_NilValue,R_GlobalEnv);
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
    //void autoloads(void){

    SEXP da, dacall, al, alcall, AutoloadEnv, name, package;
    int i,j, idx=0, errorOccurred, ptct;

    /* delayedAssign call*/
    PROTECT(da = Rf_findFun(Rf_install("delayedAssign"), R_GlobalEnv));
    PROTECT(AutoloadEnv = Rf_findVar(Rf_install(".AutoloadEnv"), R_GlobalEnv));
    if (AutoloadEnv == R_NilValue){
	fprintf(stderr,"%s: Cannot find .AutoloadEnv!\n", programName);
	exit(1);
    }
    PROTECT(dacall = allocVector(LANGSXP,5));
    SETCAR(dacall,da);
    /* SETCAR(CDR(dacall),name); */          /* arg1: assigned in loop */
    /* SETCAR(CDR(CDR(dacall)),alcall); */  /* arg2: assigned in loop */
    SETCAR(CDR(CDR(CDR(dacall))),R_GlobalEnv); /* arg3 */
    SETCAR(CDR(CDR(CDR(CDR(dacall)))),AutoloadEnv); /* arg3 */

    /* autoloader call */
    PROTECT(al = Rf_findFun(Rf_install("autoloader"), R_GlobalEnv));
    PROTECT(alcall = allocVector(LANGSXP,3));
    SET_TAG(alcall, R_NilValue); /* just like do_ascall() does */
    SETCAR(alcall,al);
    /* SETCAR(CDR(alcall),name); */          /* arg1: assigned in loop */
    /* SETCAR(CDR(CDR(alcall)),package); */  /* arg2: assigned in loop */

    ptct = 5;
    for(i = 0; i < packc; i++){
	idx += (i != 0)? packobjc[i-1] : 0;
	for (j = 0; j < packobjc[i]; j++){
	    /*printf("autload(%s,%s)\n",packobj[idx+j],pack[i]);*/

	    PROTECT(name = NEW_CHARACTER(1));
	    PROTECT(package = NEW_CHARACTER(1));
	    SET_STRING_ELT(name, 0, COPY_TO_USER_STRING(packobj[idx+j]));
	    SET_STRING_ELT(package, 0, COPY_TO_USER_STRING(pack[i]));

	    /* Set up autoloader call */
	    PROTECT(alcall = allocVector(LANGSXP,3));
	    SET_TAG(alcall, R_NilValue); /* just like do_ascall() does */
	    SETCAR(alcall,al);
	    SETCAR(CDR(alcall),name);
	    SETCAR(CDR(CDR(alcall)),package);

	    /* Setup delayedAssign call */
	    SETCAR(CDR(dacall),name);
	    SETCAR(CDR(CDR(dacall)),alcall);

	    R_tryEval(dacall,R_GlobalEnv,&errorOccurred);
	    if (errorOccurred){
		fprintf(stderr,"%s: Error calling delayedAssign!\n", 
			programName);
		exit(1);
	    }

	    ptct += 3;
	}
    }
    UNPROTECT(ptct);
}

int RInside::parseEval(const std::string & line, SEXP & ans) {
    ParseStatus status;
    SEXP cmdSexp, cmdexpr = R_NilValue;
    int i, errorOccurred;

    mb_m.add((char*)line.c_str());
    
    PROTECT(cmdSexp = allocVector(STRSXP, 1));
    SET_STRING_ELT(cmdSexp, 0, mkChar((char*)mb_m.getBufPtr()));

    cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));

    switch (status){
    case PARSE_OK:
	/* Loop is needed here as EXPSEXP might be of length > 1 */
	for(i = 0; i < length(cmdexpr); i++){
	    ans = R_tryEval(VECTOR_ELT(cmdexpr, i),NULL,&errorOccurred);
	    if (errorOccurred) {
		fprintf(stderr, "%s: Error in evaluating R code (%d)\n", programName, status);
		UNPROTECT(2);
		return 1;
	    }
	    if (verbose_m) {
		PrintValue(ans);
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

// assign for vector< vector< double > >
void RInside::assign(const std::vector< std::vector< double > > & mat, const std::string & nam) {
    int nx = mat.size();
    int ny = mat[0].size();
    SEXP sexpmat = PROTECT(allocMatrix(REALSXP, nx, ny));
    for(int i = 0; i < nx; i++) {
	for(int j = 0; j < ny; j++) {
	    REAL(sexpmat)[i + nx*j] = mat[i][j];
	}
    }
    setVar(install((char*) nam.c_str()), sexpmat, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}

// assign for vector< vector< int > >
void RInside::assign(const std::vector< std::vector< int > > & mat, const std::string & nam) {
    int nx = mat.size();
    int ny = mat[0].size();
    SEXP sexpmat = PROTECT(allocMatrix(INTSXP, nx, ny));
    for(int i = 0; i < nx; i++) {
	for(int j = 0; j < ny; j++) {
	    INTEGER(sexpmat)[i + nx*j] = mat[i][j];
	}
    }
    setVar(install((char*) nam.c_str()), sexpmat, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}

// assign for vector< double > 
void RInside::assign(const std::vector< double > & vec, const std::string & nam) {
    int nx = vec.size();
    SEXP sexpvec = PROTECT(allocVector(REALSXP, nx));
    for(int i = 0; i < nx; i++) {
	REAL(sexpvec)[i] = vec[i];
    }
    setVar(install((char*) nam.c_str()), sexpvec, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}

// assign for vector< string > 
void RInside::assign(const std::vector< std::string > & vec, const std::string & nam) {
    int len = (int)vec.size();
    SEXP sexpvec = PROTECT(allocVector(STRSXP, len));
    for (int i = 0; i < len; i++) {
        SET_STRING_ELT(sexpvec, i, mkChar(vec[i].c_str()));
    }
    setVar(install((char*) nam.c_str()), sexpvec, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}


// assign for vector< int > 
void RInside::assign(const std::vector< int > & vec, const std::string & nam) {
    int nx = vec.size();
    SEXP sexpvec = PROTECT(allocVector(INTSXP, nx));
    for(int i = 0; i < nx; i++) {
	INTEGER(sexpvec)[i] = vec[i];
    }
    setVar(install((char*) nam.c_str()), sexpvec, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}

void RInside::assign(const std::string & txt, const std::string & nam) {
    SEXP value = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(value, 0, mkChar(txt.c_str()));
    setVar(install((char*) nam.c_str()), value, R_GlobalEnv);  // now set it
    UNPROTECT(1);
}
