
# ## Use R's internal knowledge of path settings to find the lib/ directory
# ## plus optinally an arch-specific directory on system building multi-arch
# RInsideLdPath <- function() {
#     Rcpp:::packageLibPath( package = "RInside" )
# }

## Provide linker flags -- i.e. -L/path/to/libRInside -- as well as an                           
## optional rpath call needed to tell the Linux dynamic linker about the
## location.  This is not needed on OS X where we encode this as library
## built time (see src/Makevars) or Windows where we use a static library
RInsideLdFlags <- function(static=Rcpp:::staticLinking()) {
    Rcpp:::packageLdFlags( "RInside", static )
}

## Provide compiler flags -- i.e. -I/path/to/RInside.h
RInsideCxxFlags <- function() {
	Rcpp:::includeFlag( package = "RInside" )
}

## Shorter names, and call cat() directly
CxxFlags <- function() cat(RInsideCxxFlags())
LdFlags <- function(static=Rcpp:::staticLinking()) cat(RInsideLdFlags(static))

