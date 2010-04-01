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
#include "Callbacks.h"

RInside* RInside::instance_ = 0 ;

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
    instance_ = 0 ;
}

RInside::RInside() : callbacks(0) {
	initialize( 0, 0 );
}

RInside::RInside(const int argc, const char* const argv[]) : callbacks(0) {
    initialize( argc, argv ); 
}

// TODO: use a vector<string> would make all this a bit more readable 
void RInside::initialize(const int argc, const char* const argv[]){
    logTxt("RInside::ctor BEGIN", verbose);

    if( instance_ ){
    	throw std::runtime_error( "can only have one RInside instance" ) ;
    } else {
    	instance_ = this ;	
    }
    
    verbose_m = false; 		// Default is false

    // generated as littler.h via from svn/littler/littler.R
    #include "RInsideEnvVars.h"

    for (int i = 0; R_VARS[i] != NULL; i+= 2) {
	if (getenv(R_VARS[i]) == NULL) { // if env variable is not yet set
	    if (setenv(R_VARS[i],R_VARS[i+1],1) != 0){
		//perror("ERROR: couldn't set/replace an R environment variable");
		//exit(1);
		throw std::runtime_error(std::string("Could not set R environment variable ") +
					 std::string(R_VARS[i]) + std::string(" to ") +  
					 std::string(R_VARS[i+1]));
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
	//perror("Fatal Error: couldn't set/replace R_SESSION_TMPDIR!");
	//exit(1);
	throw std::runtime_error(std::string("Could not set / replace R_SESSION_TMPDIR to ") + std::string(tmp));
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
 
    // Autoload default packages and names from autoloads.h
    //
    // This function behaves in almost every way like
    // R's autoload:
    // function (name, package, reset = FALSE, ...)
    // {
    //     if (!reset && exists(name, envir = .GlobalEnv, inherits = FALSE))
    //        stop("an object with that name already exists")
    //     m <- match.call()
    //     m[[1]] <- as.name("list")
    //     newcall <- eval(m, parent.frame())
    //     newcall <- as.call(c(as.name("autoloader"), newcall))
    //     newcall$reset <- NULL
    //     if (is.na(match(package, .Autoloaded)))
    //        assign(".Autoloaded", c(package, .Autoloaded), env = .AutoloadEnv)
    //     do.call("delayedAssign", list(name, newcall, .GlobalEnv,
    //                                                         .AutoloadEnv))
    //     invisible()
    // }
    //
    // What's missing is the updating of the string vector .Autoloaded with 
    // the list of packages, which by my code analysis is useless and only 
    // for informational purposes.
    //
    //
    
    // we build the call : 
    //
    //  delayedAssign( NAME, 
    //  	autoloader( name = NAME, package = PACKAGE), 
    //  	.GlobalEnv, 
    //  	.AutoloadEnv )
    //  	
    //  where : 
    //  - PACKAGE is updated in a loop
    //  - NAME is updated in a loop
    //  
    //
     
    int i,j, idx=0, nobj ;
    Rcpp::Language delayed_assign_call(Rcpp::Function("delayedAssign"), 
				       R_NilValue,     // arg1: assigned in loop 
				       R_NilValue,     // arg2: assigned in loop 
				       global_env,
				       global_env.find(".AutoloadEnv")
				       ) ;
    Rcpp::Language::Proxy delayed_assign_name  = delayed_assign_call[1];

    Rcpp::Language autoloader_call(Rcpp::Function("autoloader"),
				   Rcpp::Named( "name", R_NilValue) ,  // arg1 : assigned in loop 
				   Rcpp::Named( "package", R_NilValue) // arg2 : assigned in loop 
				   );
    Rcpp::Language::Proxy autoloader_name = autoloader_call[1];
    Rcpp::Language::Proxy autoloader_pack = autoloader_call[2];
    delayed_assign_call[2] = autoloader_call ;
    
    try { 
    	for( i=0; i<packc; i++){
    		
	    // set the 'package' argument of the autoloader call */
	    autoloader_pack = pack[i] ;
		
	    nobj = packobjc[i] ; 
	    for (j = 0; j < nobj ; j++){
		
		// set the 'name' argument of the autoloader call */ 
		autoloader_name = packobj[idx+j] ;
		   
		// Set the 'name' argument of the delayedAssign call */
		delayed_assign_name = packobj[idx+j] ;
		    
		// evaluate the call */
		delayed_assign_call.eval() ;
		    
	    }
	    idx += packobjc[i] ;
    	}
    } catch( std::exception& ex){
	// fprintf(stderr,"%s: Error calling delayedAssign:\n %s", programName, ex.what() );
	// exit(1);	    
	throw std::runtime_error(std::string("Error calling delayedAssign: ") + std::string(ex.what()));
    }
}

// this is a non-throwing version returning an error code
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
	// Loop is needed here as EXPSEXP might be of length > 1 
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
	// need to read another line 
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

void RInside::parseEvalQ(const std::string & line) {
    SEXP ans;
    int rc = parseEval(line, ans);
    if (rc != 0) {
	throw std::runtime_error(std::string("Error evaluating: ") + line);
    }
}

RInside::Proxy RInside::parseEval(const std::string & line) {
    SEXP ans;
    int rc = parseEval(line, ans);
    if (rc != 0) {
	throw std::runtime_error(std::string("Error evaluating: ") + line);
    }
    return Proxy( ans );
}

Rcpp::Environment::Binding RInside::operator[]( const std::string& name ){
    return global_env[name]; 
}

RInside& RInside::instance(){
	return *instance_ ;
}

/* callbacks */

void Callbacks::busy_( int which ){
	R_is_busy = static_cast<bool>( which ) ;
	busy( R_is_busy ) ;	
}

int Callbacks::readConsole_( const char* prompt, unsigned char* buf, int len, int addtohistory ){
	try {
		std::string res( readConsole( prompt, static_cast<bool>(addtohistory) ) ) ;
		
		/* At some point we need to figure out what to do if the result is
		 * longer than "len"... For now, just truncate. */
		 
		int l = res.size() ;
		int last = (l>len-1)?len-1:l ;
		strncpy( (char*)buf, res.c_str(), last ) ;
		buf[last] = 0 ;
		return 1 ;
	} catch( const std::exception& ex){
		return -1 ;	
	}
}


void Callbacks::writeConsole_( const char* buf, int len, int oType ){
	std::string line ;
	line.assign( buf, buf + len ) ;
	writeConsole( line, oType) ;
}

void RInside_ShowMessage( const char* message ){
	RInside::instance().callbacks->showMessage( message ) ;	
}

void RInside_WriteConsoleEx( const char* message, int len, int oType ){
	RInside::instance().callbacks->writeConsole_( message, len, oType ) ;		
}

int RInside_ReadConsole(const char *prompt, unsigned char *buf, int len, int addtohistory){
	return RInside::instance().callbacks->readConsole_( prompt, buf, len, addtohistory ) ;
}

void RInside_ResetConsole(){
	RInside::instance().callbacks->resetConsole() ;
}

void RInside_FlushConsole(){                                       
	RInside::instance().callbacks->flushConsole() ;
}

void RInside_ClearerrConsole(){
	RInside::instance().callbacks->cleanerrConsole() ;
}

void RInside_Busy( int which ){
	RInside::instance().callbacks->busy_(which) ;
}

void RInside::set_callbacks(Callbacks* callbacks_){
	Rprintf( "<set_callbacks>\n" ) ;
	if( ! callbacks ){
		/* short circuit the callback function pointers */
		Rprintf( "ptr_R_ShowMessage : <%p> = <%p>\n", ptr_R_ShowMessage, RInside_ShowMessage ) ;
		ptr_R_ShowMessage = RInside_ShowMessage ;
		
		Rprintf( "ptr_R_ReadConsole : <%p> = <%p>\n", ptr_R_ReadConsole, RInside_ReadConsole ) ;
		ptr_R_ReadConsole = RInside_ReadConsole;
    	
		// Rprintf( "ptr_R_WriteConsoleEx : <%p> = <%p>\n", ptr_R_WriteConsoleEx, RInside_WriteConsoleEx ) ;
		// ptr_R_WriteConsoleEx = RInside_WriteConsoleEx;
		// ptr_R_WriteConsole = NULL;
		Rprintf( "after\n") ;
		
		
		Rprintf( "ptr_R_FlushConsole : <%p> = <%p>\n", ptr_R_FlushConsole, RInside_FlushConsole ) ;
		ptr_R_FlushConsole = RInside_FlushConsole;
    	
		Rprintf( "ptr_R_ClearerrConsole : <%p> = <%p>\n", ptr_R_ClearerrConsole, RInside_ClearerrConsole ) ;
		ptr_R_ClearerrConsole = RInside_ClearerrConsole;
    	
		Rprintf( "ptr_R_Busy : <%p> = <%p>\n", ptr_R_Busy, RInside_Busy ) ;
		ptr_R_Busy = RInside_Busy;
    	
		Rprintf( "ptr_R_ResetConsole : <%p> = <%p>\n", ptr_R_ResetConsole, RInside_ResetConsole ) ;
		ptr_R_ResetConsole = RInside_ResetConsole;
    	
    	R_Outputfile = NULL;
    	R_Consolefile = NULL;    
    	
    	// setup_Rmainloop()
    	// this gives this message: 
    	// Erreur dans .Call("R_isMethodsDispatchOn", onOff, PACKAGE = "base") : 
		//   Nombre d'arguments incorrect (2), attendu 1 pour R_isMethodsDispatchOn
		// Segmentation fault

    	
    }
    // callbacks = callbacks_ ;
	// this gives a segfauylt
	
	Rprintf( "</set_callbacks>\n" ) ;
	
}

