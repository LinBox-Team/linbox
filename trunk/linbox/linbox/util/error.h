/* -*- mode: c++; style: linux -*- */

/* linbox/util/error.h
 * Copyright (C) 1994-1997 Givaro Team
 *
 * Written by T. Gautier
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __UTIL_ERROR_H
#define __UTIL_ERROR_H

#include <iostream.h>

namespace LinBox
{

// ------------------------------- LinboxError
// - Base class for execption handling in Givaro
class LinboxError {
    public:
	LinboxError(const char* msg =0 ) 
		: strg(msg) {};

	// -- virtual print of the error message
	virtual ostream& print( ostream& o )  const
		{ return o << strg ; }
  
	// -- non virtual output operator
	friend ostream& operator<< (ostream& o, const LinboxError& E);

	// - useful to setup a break point on it
	static void throw_error( const LinboxError& err )
		{ throw err; }

    protected:
	const char* strg;  
};

class LinboxMathError : public LinboxError {
 public:
	LinboxMathError(const char* msg) : LinboxError(msg) {};
};

class LinboxMathDivZero : public LinboxMathError {
 public:
	LinboxMathDivZero(const char* msg) : LinboxMathError(msg) {};
};


// -- Exception thrown in input of data structure 
class LinboxBadFormat : public LinboxError {
 public:
	LinboxBadFormat(const char* msg) : LinboxError(msg) {};
};
 
}

#ifdef LinBoxSrcOnly
// for all-source compilation
#include <linbox/util/error.C>
#endif

#endif // __UTIL_ERROR_H
