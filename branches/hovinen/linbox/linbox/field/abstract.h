/* -*- mode: c; style: linux -*- */

/* linbox/src/field/abstract.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
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

#ifndef __FIELD_ABSTRACT_H
#define __FIELD_ABSTRACT_H

#include <iostream>

#include "linbox/element/abstract.h"
#include "linbox/randiter/abstract.h"
#include "linbox/integer.h"

namespace LinBox 
{ 

	/** Abstract field base class.
	 * Found in the file \URL{src/library/archetypes/field/field_abstract.h}.
	 * Abstract base class used to implement the field archetype to minimize
	 * code bloat.  All public member functions of this class are purely
	 * virtual and must be implemented by all derived classes.
	 *
	 * If a template is instantiated on the field archetype, we can change the
	 * field it is using by changing the derived class of this class.  This allows
	 * us to change the field used in a template without having to reinstantiate
	 * it.  This minimizes code bloat, but it also introduces indirection through
	 * the use of pointers and virtual functions which is inefficient.
	 */
	class Field_abstract
	{
	    public:

		/// Element type.
		typedef Element_abstract Element;

		/// Random iterator generator type.
		typedef RandIter_abstract RandIter;
 
		/** @name Object Management
		 * There are no public constructors for this class.
		 * It should only be used in tandem with \Ref{Field_archetype}.
		 */
		//@{

		/** Destructor.
		 * Required because of virtual member functions.
		 * Virtual.
		 */
		virtual ~Field_abstract (void) {}

		/** Virtual copy constructor.
		 * Required because constructors cannot be virtual.
		 * Passes construction on to derived classes.
		 * Purely virtual.
		 * This function is not part of the common object interface.
		 * @return pointer to new object in dynamic memory.
		 */
		virtual Field_abstract* clone () const = 0;

		/** Assignment operator.
		 * Purely virtual.
		 * @return reference to self
		 * @param F constant reference to Field_abstract object
		 */
		virtual Field_abstract &operator= (const Field_abstract &F) = 0;

		/** Initialization of field element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field element x has already been
		 * constructed, but that it is not already initialized.
		 * Purely virtual.
		 * @return reference to field element.
		 * @param x field element to contain output (reference returned).
		 * @param y integer.
		 */
		virtual Element &init (Element &x, const Integer &y) const = 0;
 
		/** Conversion of field element to an integer.
		 * This function assumes the output field element x has already been
		 * constructed, but that it is not already initialized.
		 * Purely virtual.
		 * @return reference to integer.
		 * @param x reference to interger to contain output (reference returned).
		 * @param y constant field element.
		 */
		virtual Integer &convert (Integer &x, const Element &y) const = 0;
 
		/** Assignment of one field element to another.
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &assign (Element &x, const Element &y) const = 0;

		/** Cardinality.
		 * Return integer representing cardinality of the domain.
		 * Returns a non-negative integer for all domains with finite
		 * cardinality, and returns -1 to signify a domain of infinite
		 * cardinality.
		 * Purely virtual.
		 * @return integer representing cardinality of the domain
		 */
		virtual Integer &cardinality (Integer &c) const = 0;
 
		/** Characteristic.
		 * Return integer representing characteristic of the domain.
		 * Returns a positive integer to all domains with finite characteristic,
		 * and returns 0 to signify a domain of infinite characteristic.
		 * Purely virtual.
		 * @return integer representing characteristic of the domain.
		 */
		virtual Integer &characteristic (Integer &c) const = 0;

		//@} Object Management

		/** @name Arithmetic Operations
		 * x <- y op z; x <- op y
		 * These operations require all elements, including x, to be initialized
		 * before the operation is called.  Uninitialized field elements will
		 * give undefined results.
		 */
		//@{

		/** Equality of two elements.
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return boolean true if equal, false if not.
		 * @param  x field element
		 * @param  y field element
		 */
		virtual bool areEqual (const Element &x, const Element &y) const = 0;

		/** Addition.
		 * x = y + z
		 * This function assumes all the field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		virtual Element &add (Element       &x,
				      const Element &y,
				      const Element &z) const = 0;
 
		/** Subtraction.
		 * x = y - z
		 * This function assumes all the field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		virtual Element &sub (Element       &x,
				      const Element &y,
				      const Element &z) const = 0;
 
		/** Multiplication.
		 * x = y * z
		 * This function assumes all the field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		virtual Element &mul (Element       &x,
				      const Element &y,
				      const Element &z) const = 0;
 
		/** Division.
		 * x = y / z
		 * This function assumes all the field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		virtual Element &div (Element &x,
				      const Element &y, const Element &z) const = 0;
 
		/** Additive Inverse (Negation).
		 * x = - y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &neg (Element &x, const Element &y) const = 0;
 
		/** Multiplicative Inverse.
		 * x = 1 / y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &inv (Element &x, const Element &y) const = 0;

		/** Natural AXPY.
		 * r  = a * x + y
		 * This function assumes all field elements have already been 
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to r.
		 * @param  r field element (reference returned).
		 * @param  a field element.
		 * @param  x field element.
		 * @param  y field element.
		 */
		virtual Element &axpy (Element       &r, 
				       const Element &a, 
				       const Element &x, 
				       const Element &y) const = 0;

		//@} Arithmetic Operations
 
		/** @name Inplace Arithmetic Operations
		 * x <- x op y; x <- op x
		 */
		//@{

		/** Zero equality.
		 * Test if field element is equal to zero.
		 * This function assumes the field element has already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return boolean true if equals zero, false if not.
		 * @param  x field element.
		 */
		virtual bool isZero (const Element &x) const = 0;
 
		/** One equality.
		 * Test if field element is equal to one.
		 * This function assumes the field element has already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return boolean true if equals one, false if not.
		 * @param  x field element.
		 */
		virtual bool isOne (const Element &x) const = 0;

		/** Inplace Addition.
		 * x += y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &addin (Element &x, const Element &y) const = 0;
 
		/** Inplace Subtraction.
		 * x -= y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &subin (Element &x, const Element &y) const = 0;
 
		/** Inplace Multiplication.
		 * x *= y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &mulin (Element &x, const Element &y) const = 0;

		/** Inplace Division.
		 * x /= y
		 * This function assumes both field elements have already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		virtual Element &divin (Element &x, const Element &y) const = 0;
 
		/** Inplace Additive Inverse (Inplace Negation).
		 * x = - x
		 * This function assumes the field element has already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 */
		virtual Element &negin (Element &x) const = 0;
 
		/** Inplace Multiplicative Inverse.
		 * x = 1 / x
		 * This function assumes the field elementhas already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 */
		virtual Element &invin (Element &x) const = 0;

		/** Inplace AXPY.
		 * r  += a * x
		 * This function assumes all field elements have already been 
		 * constructed and initialized.
		 * Purely virtual
		 * @return reference to r.
		 * @param  r field element (reference returned).
		 * @param  a field element.
		 * @param  x field element.
		 */
		virtual Element &axpyin (Element       &r, 
					 const Element &a, 
					 const Element &x) const = 0;
 
		//@} Inplace Arithmetic Operations

		/** @name Input/Output Operations */
		//@{

		/** Print field.
		 * Purely virtual.
		 * @return output stream to which field is written.
		 * @param  os  output stream to which field is written.
		 */
		virtual ostream &write (ostream &os) const = 0;
 
		/** Read field.
		 * Purely virtual.
		 * @return input stream from which field is read.
		 * @param  is  input stream from which field is read.
		 */
		virtual istream &read (istream &is) = 0;

		/** Print field element.
		 * This function assumes the field element has already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return output stream to which field element is written.
		 * @param  os  output stream to which field element is written.
		 * @param  x   field element.
		 */
		virtual ostream &write (ostream &os, const Element &x) const = 0;
 
		/** Read field element.
		 * This function assumes the field element has already been
		 * constructed and initialized.
		 * Purely virtual.
		 * @return input stream from which field element is read.
		 * @param  is  input stream from which field element is read.
		 * @param  x   field element.
		 */
		virtual istream &read (istream &is, Element &x) const = 0;

		//@}

	    protected:

		/** Default Constructor.
		 * Required by derived classes, but protected because this class should
		 * never be constructed by itself.
		 */
		Field_abstract () {}
 
	    private:

		/// Field_archetype is friend.
		friend class Field_archetype;

	}; // class Field_abstract

} // namespace LinBox

#endif // __FIELD_ABSTRACT_H
