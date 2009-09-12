
## Use R's internal knowledge of path settings to find the lib/ directory
## plus optinally an arch-specific directory on system building multi-arch
RInsideLdPath <- function() {
    if (nzchar(.Platform$r_arch)) {	## eg amd64, ia64, mips
        system.file("lib",.Platform$r_arch,package="RInside")
    } else {
        system.file("lib",package="RInside")
    }
}

## Provide linker flags -- i.e. -L/path/to/libRInside -- as well as an
## optional rpath call needed to tell the Linux dynamic linker about the
## location.  This is not needed on OS X where we encode this as library
## built time (see src/Makevars) or Windows where we use a static library
RInsideLdFlags <- function(static=FALSE) {
    rinsidedir <- RInsideLdPath()
    flags <- paste("-L", rinsidedir, " -lRInside", sep="")	## general default
    if (.Platform$OS.type == "unix") {
        if (length(grep("^linux",R.version$os))) {
            if (static==FALSE) {		## on Linux with dyn. linking, use rpath too
                flags <- paste(flags, " -Wl,-rpath,", rinsidedir, sep="")
            }
        }
    }
    invisible(flags)
}

## Provide compiler flags -- i.e. -I/path/to/RInside.h
RInsideCxxFlags <- function() {
    paste("-I", RInsideLdPath(), sep="")
}

## Shorter names, and call cat() directly
CxxFlags <- function() cat(RInsideCxxFlags())
LdFlags <- function() cat(RInsideLdFlags())

