/* -*- mode: c; style: linux -*- */

/* linbox/field/ntl-RR.h
 * Copyright (C) 1999-2002 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __FIELD_NTL_RR_H
#define __FIELD_NTL_RR_H

#include <NTL/RR.h>

#include "linbox/field/unparametric.h"

// Namespace in which all LinBox library code resides
namespace LinBox
{
  
	/** @name class RR.
	 * Arbitrary precision floating point numbers.
	 * These specializations allow the \Ref{UnparametricField} template class to be
	 * used to wrap NTL's RR class as a LinBox field.
	 */
	//@{

	/** Initialization of field element from an integer.
	 * Behaves like C++ allocator construct.
	 * This function assumes the output field element x has already been
	 * constructed, but that it is not already initialized.
	 * For now, this is done by converting the integer type to a C++
	 * long and then to the element type through the use of static cast and
	 * NTL's to_RR function.
	 * This, of course, assumes such static casts are possible.
	 * This function should be changed in the future to avoid using long.
	 * @return reference to field element.
	 * @param x field element to contain output (reference returned).
	 * @param y integer.
	 */
	template <>
		NTL::RR& UnparametricField<NTL::RR>::init(NTL::RR& x, const integer& y) const
		{ return x = NTL::to_RR(static_cast<const long&>(y)); }

	/** Conversion of field element to an integer.
	 * This function assumes the output field element x has already been
	 * constructed, but that it is not already initialized.
	 * For now, this is done by converting the element type to a C++
	 * long and then to the integer type through the use of static cast and
	 * NTL's to_long function.
	 * This, of course, assumes such static casts are possible.
	 * This function should be changed in the future to avoid using long.
	 * @return reference to integer.
	 * @param x reference to integer to contain output (reference returned).
	 * @param y constant reference to field element.
	 */
	template <>
		integer& UnparametricField<NTL::RR>::convert(integer& x, const NTL::RR& y) const
		{ return x = static_cast<integer>(to_long(y)); }

	/** Multiplicative Inverse.
	 * x = 1 / y
	 * This function assumes both field elements have already been
	 * constructed and initialized.
	 * @return reference to x.
	 * @param  x field element (reference returned).
	 * @param  y field element.
	 */
	template <> 
		NTL::RR& UnparametricField<NTL::RR>::inv(NTL::RR& x, const NTL::RR& y) const
		{ return x = NTL::inv(y); }
 
	/** Zero equality.
	 * Test if field element is equal to zero.
	 * This function assumes the field element has already been
	 * constructed and initialized.
	 * In this specialization, NTL's IsZero function is called.
	 * @return boolean true if equals zero, false if not.
	 * @param  x field element.
	 */
	template <> bool UnparametricField<NTL::RR>::isZero(const NTL::RR& x) const
		{ return static_cast<bool>(IsZero(x)); }

	/** One equality.
	 * Test if field element is equal to one.
	 * This function assumes the field element has already been
	 * constructed and initialized.
	 * In this specialization, NTL's IsOne function is called.
	 * @return boolean true if equals one, false if not.
	 * @param  x field element.
	 */
	template <> bool UnparametricField<NTL::RR>::isOne(const NTL::RR& x) const
		{ return static_cast<bool>(IsOne(x)); }

	/** Inplace Multiplicative Inverse.
	 * x = 1 / x
	 * This function assumes both field elements have already been
	 * constructed and initialized.
	 * @return reference to x.
	 * @param  x field element (reference returned).
	 */
	template <> NTL::RR& UnparametricField<NTL::RR>::invin(NTL::RR& x) const
		{ return x = NTL::inv(x); }

	/** Print field.
	 * @return output stream to which field is written.
	 * @param  os  output stream to which field is written.
	 */
	template <> ostream& UnparametricField<NTL::RR>::write(ostream& os) const 
		{ return os << "unparamterized field NTL::RR"; }

	/** Random field element creator.
	 * This returns a random field element from the information supplied
	 * at the creation of the generator.
	 * This generator uses the built-in C++ random number generator instead of
	 * NTL's random function because the NTL function does not allow as much
	 * control over the sampling size as the generic LinBox template.  This 
	 * specialization is included only to allow conversion to an NTL 
	 * object.
	 * @return random field element
	 */
	template <> NTL::RR& UnparametricRandIter<NTL::RR>::random(NTL::RR &elt)
		{
			double temp;
    
			// Create new random elements
			if (_size == 0)
				elt = rand();
			else
				elt = static_cast<long>((double(rand())/RAND_MAX)*double(_size));

			elt = temp;

#ifdef TRACE
			cout << "random double = " << temp 
			     << "    random NTL::RR = " << elt << endl;
#endif // TRACE

			return elt;
    
		} // element& operator() (void)

	//@} 
} // namespace LinBox

#endif // __FIELD_NTL_RR_H
