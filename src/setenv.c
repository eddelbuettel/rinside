// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: Easier R embedding into C++
//
// Copyright (C) 2009 Dirk Eddelbuettel and Richard Holbrey
// and GPL'ed

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// borrowed from Renviron.c
extern "C" int setenv(const char *env_var, const char *env_val, int dummy) {
    char *buf, *value, *p, *q, *a, *b, quote='\0';
    int inquote = 0;

    //make non-const copies
    a = (char *) malloc((strlen(env_var) + 1) * sizeof(char));
    b = (char *) malloc((strlen(env_val) + 1) * sizeof(char));
    if (!a || !b) {
	fprintf(stderr, "memory allocation failure in setenv");
        exit(1);
    }
    strcpy(a, env_var);
    strcpy(b, env_val);

    buf = (char *) malloc((strlen(a) + strlen(b) + 2) * sizeof(char));
    if (!buf) {
	fprintf(stderr, "memory allocation failure in setenv");
        exit(1);
    }
    strcpy(buf, a); strcat(buf, "=");
    value = buf+strlen(buf);

    /* now process the value */
    for(p = b, q = value; *p; p++) {
	/* remove quotes around sections, preserve \ inside quotes */
	if(!inquote && (*p == '"' || *p == '\'')) {
	    inquote = 1;
	    quote = *p;
	    continue;
	}

	if(inquote && *p == quote && *(p-1) != '\\') {
	    inquote = 0;
	    continue;
	}

	if(!inquote && *p == '\\') {
	    if(*(p+1) == '\n') p++;
	    else if(*(p+1) == '\\') *q++ = *p;
	    continue;
	}

	if(inquote && *p == '\\' && *(p+1) == quote)
	    continue;
	*q++ = *p;
    }
    *q = '\0';
    //if (putenv(buf)) 
	//warningcall(R_NilValue, _("problem in setting variable '%s' in Renviron"), a);
    return putenv(buf);

    /* no free here: storage remains in use */
}

