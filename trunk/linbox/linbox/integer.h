/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* Copyright(c)'94-97 by Givaro Team
 * Copyright(c)'2000-2002 by LinBox Team 
 * see the copyright file.
 * Created by M. Samama, T. Gautier
 *
 * Modified Jean-Guillaume.Dumas <Jean-Guillaume.Dumas@imag.fr>
 *          B. David Saunders <saunders@cis.udel.edu>,
 *          Bradford Hovinen <hovinen@cis.udel.edu>
 *          Gilles Villard <Gilles.Villard@ens-lyon.fr>
 *                        JGD Random functions back.                          
 *                        (2002/02/12 16:05:24) 
 *
 */

#ifndef __INTEGER_H
#define __INTEGER_H

#include "linbox-config.h"

#include "gmp++/gmp++.h"

namespace LinBox
{
	/* @name integer  */
	/** 
	 * @memo This is a representation of arbitrary integers.  
	 * @doc It is a wrapper of GMP integers.  Arithmetic operations are via
C++ infix operator forms (eg. a*b) . It is for ``casual'' uses such as characteristics and
cardinalities and when initializing field elements.  For the integers as a LinBox
ring for use in integer matrix computation, see gmp-rational.h or ntl-ZZ.h.
	 */ 
	typedef Integer integer;

	typedef signed __LINBOX_INT8 int8;
	typedef signed __LINBOX_INT16 int16;

	/** @name int32 
	 * @memo This is a representation of 32 bit ints, usually equivalent to `int'.
	 * @doc The use of `int32' ensures you are working with 
	 * 32 bit signed ints, [-2^31..2^31).  Similarly, int8, int16, and int64 are defined.
	 */
	typedef signed __LINBOX_INT32 int32;

	typedef signed __LINBOX_INT64 int64;

	typedef unsigned __LINBOX_INT8 uint8;
	typedef unsigned __LINBOX_INT16 uint16;

	/** @name uint32 
	 * @memo This is a representation of 32 bit unsigned ints, usually equivalent to `unsigned int'.
	 * @doc The use of `uint32' ensures you are working with 
	 * 32 bit unsigned ints, [0..2^32).  Similarly, uint8, uint16, and uint64 are defined.
	 */
	typedef unsigned __LINBOX_INT32 uint32;

	typedef unsigned __LINBOX_INT64 uint64;

	template< class T >
	T abs( const T& a ) { return( a <= 0 ? a * -1 : a ); }
}

#endif // __INTEGER_H
