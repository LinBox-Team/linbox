/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/element/gmp-rational.h
 * Copyright (C) 2001-2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
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

#ifndef __ELEMENT_GMP_RATIONAL_H
#define __ELEMENT_GMP_RATIONAL_H

#include "linbox/integer.h"

#include <gmp.h>

namespace LinBox
{

	// Forward declarations
	class GMPRationalField;
	class GMPRationalRandIter;

	/** element archetype.
	 * Archetype for the element common object interface for \Ref{LinBox}.
	 *
	 * This class must contain a default constructor, a copy constructor, 
	 * an assignment operator, and a destructor.  This is to allow field 
	 * elements to be primitive C++ types such as double and int.  The copy
	 * constructor is also used to allow elements to be passed by value to 
	 * a function.
	 */
	class GMPRationalElement
	{
	    public:

		/** @name Common Object Interface for LinBox Field elements.
		 * These methods are required of all \Ref{LinBox} 
		 * {@link Fields field} elements.
		 */
		//@{

		/** Default constructor.
		 * This constructor is required to allow 
		 * {@link Fields field} elements to be primitive C++ types.
		 * Because constructor does not know what {@link Fields field} 
		 * the element belongs to, it cannot actually construct the element.
		 * In this implementation, the constructor it sets _elem_ptr
		 * to the null pointer.  Initialization of the element is done through
		 * the field function init where the field is known.
		 */
		GMPRationalElement(void) { mpq_init (rep); }

		/** Copy constructor.
		 * This constructor is required to allow 
		 * {@link Fields field} elements to be primitive C++ types, 
		 * and to allow field elements to be passed by value into 
		 * functions.
		 * Constructs {@link Fields field} element by copying the 
		 * {@link Fields field} element.
		 * In this implementation, this means copying the element to
		 * which a._elem_ptr points.
		 * @param  a field element.
		 */
		GMPRationalElement(const GMPRationalElement& a) 
			{ mpq_init (rep); mpq_set (rep, a.rep); }

		/** Destructor.
		 * In this implementation, this destroys element by deleting field 
		 * element to which _elem_ptr points.
		 */
		~GMPRationalElement() { mpq_clear (rep); }

		/** Assignment operator.
		 * Assigns element a to element.  
		 * In this implementation, this is done 
		 * by copying field element to which _elem_ptr points.
		 * @param  a field element.
		 */
		GMPRationalElement& operator=(const GMPRationalElement& a)
			{
				if (this != &a) { // guard against self-assignment
					mpq_set (rep, a.rep);
				}
				return *this;
			}

		//@} Common Object Interface

		/** @name Implementation-Specific Methods.
		 * These methods are not required of all LinBox field elements
		 * and are included only for this implementation of the archetype.
		 */
		//@{

		/** Constructor.
		 * Constructs field element from an mpq_t
		 * Not part of the interface.
		 * Creates new copy of element object in dynamic memory.
		 * @param  elem_ptr  pointer to \Ref{ElementAbstract}
		 */
		GMPRationalElement (mpq_t _rep) {
			mpq_init (rep);
			mpq_set (rep, _rep);
		}

		/** Constructor
		 * Initialize from numerator and denominator
		 */
		GMPRationalElement (const integer &num, const integer &den) 
			{
				mpq_init (rep);
				mpz_set_si (mpq_numref (rep), num);
				mpz_set_si (mpq_denref (rep), den);
			}

		//@}p
    
	    private:

		friend class GMPRationalField;
		friend class GMPRationalRandIter;

		/** @name Implementation-Specific Data.
		 * This data is not required of all LinBox field elements
		 * and is included only for this implementation of the archetype.
		 */
		//@{
    
		/** Pointer to parameterized field element.
		 * Not part of the common object interface for \Ref{LinBox} field elements.
		 * Included to avoid code bloat.
		 */
		mutable mpq_t rep;
    
		//@} Non-Interface
	};
}

#endif // _GMP_RATIONAL_NUMBER_
