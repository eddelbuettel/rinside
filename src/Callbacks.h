// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// Callbacks.h: R/C++ interface class library -- Easier R embedding into C++
//
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

#ifndef RINSIDE_CALLBACKS_H
#define RINSIDE_CALLBACKS_H

#include "RInsideCommon.h"

class Callbacks {
public:
	
	Callbacks() : R_is_busy(false){} ;
	virtual ~Callbacks(){} ;
	
	virtual void showMessage(const char* message) = 0;
	virtual void suicide(const char* message) = 0;
	virtual std::string readConsole( const char* prompt, bool addtohistory ) = 0;
	virtual void writeConsole( const std::string& line, int type ) = 0;
	virtual void flushConsole() = 0;
	virtual void resetConsole() = 0;
	virtual void cleanerrConsole() = 0;
	virtual void busy( bool is_busy ) = 0 ;
	
	void busy_( int which ) ;
	int readConsole_( const char* prompt, unsigned char* buf, int len, int addtohistory ) ;
	void writeConsole_( const char* buf, int len, int oType ) ;
	
	// TODO: ShowFiles
	// TODO: ChooseFile
	// TODO: loadHistory
	// TODO: SaveHistory                                                                                      
	
private:
	bool R_is_busy ;
	             
} ;                                       

class DummyCallbacks : public Callbacks{
public:
	DummyCallbacks() : Callbacks(){}
	
	void showMessage(const char* message){
		Rprintf( ">> showMessage('%s')\n", message ) ;	
	}
	void suicide(const char* message){
		Rprintf( ">> suicide('%s')\n", message ) ;
	}
	std::string readConsole(const char* prompt, bool addtohistory){ 
		Rprintf( ">> readConsole('%s', %d)\n", prompt, addtohistory ) ;
		return " " ; 
	}
	void writeConsole( const std::string& line, int type){
		Rprintf( ">> writeConsole('%s', %d)\n", line.c_str(), type ) ;
	}
	void flushConsole(){
		Rprintf( ">> flushConsole()\n" ) ;
	}
	void resetConsole(){
		Rprintf( ">> resetConsole()\n" ) ;
	}
	void cleanerrConsole(){
		Rprintf( ">> cleanerrConsole()\n" ) ;
	}
	void busy( bool is_busy ){
		Rprintf( ">> busy(%d) \n", is_busy ) ;
	}
	
} ;

#endif
