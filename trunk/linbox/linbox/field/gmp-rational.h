/* -*- mode: c; style: linux -*- */

/* linbox/field/gmp-rational.h
 * Copyright (C) 2001, 2002 Bradford Hovinen
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

#ifndef __FIELD_GMP_RATIONAL_H
#define __FIELD_GMP_RATIONAL_H

#include <iostream>

#include "linbox/integer.h"
#include "linbox/element/gmp-rational.h"

extern "C" {
#    include <ctype.h>
}

#include <gmp.h>

// Namespace in which all LinBox library code resides
namespace LinBox
{
	// Forward declarations
	class RandIterArchetype;

	/** Field of rational numbers using GMP
	 *
	 * This is a wrapper for the GMP rational number facility, built to the
	 * interface of the field archetype. 
	 */
	class GMPRationalField
	{
	    public:
    
		/** @name Common Object Interface for a LinBox Field.
		 * These methods are required of all \Ref{LinBox} fields.
		 */
		//@{
    
		/// element type.
		typedef GMPRationalElement element;

		/// Random iterator generator type.
		typedef GMPRationalRandIter RandIter;

		/** @name Object Management
		 * x <- convert (y)
		 */
		//@{
    
		/** Copy constructor.
		 *
		 * Vacuous, since this field is unparametric so there is no need to
		 * construct multiple field objects
		 */

		GMPRationalField (const GMPRationalField &F) 
			: zero (_zero, _one), one (_one, _one), neg_one (_neg_one, _one)
			{}

		/** Destructor.
		 * 
		 * Also vacuous, since there is no de-initialization system
		 */
		~GMPRationalField (void) 
			{}
    
		/** Assignment operator.
		 * 
		 * Also vacuous
		 */
		GMPRationalField &operator= (const GMPRationalField &F)
			{ return *this; }
    
		/** Initialization of field element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field element x has already been 
		 * constructed, but that it is not necessarily already initialized.
		 * In this implementation, this means the _elem_ptr of x exists, but
		 * that it may be the null pointer.
		 * @return reference to field element.
		 * @param x field element to contain output (reference returned).
		 * @param y constant reference to integer.
		 */
		element &init (element &x, const integer &y = 0) const
		{
			mpq_set_si (x.rep, (signed long) y, 1L);
			mpq_canonicalize (x.rep);
			return x;
		}
  
		/** Conversion of field element to an integer.
		 * This function assumes the output field element x has already been 
		 * constructed, but that it is not already initialized.
		 * In this implementation, this means the _elem_ptr of y exists, and
		 * that it is not the null pointer.
		 *
		 * Returns floor (numerator (y) / denominator (y))
		 *
		 * @return reference to integer.
		 * @param x reference to integer to contain output (reference returned).
		 * @param y constant reference to field element.
		 */
		integer &convert (integer &x, const element &y = 0) const
		{
			mpz_t n, d;
			unsigned long v;

			mpz_init (n);
			mpz_init (d);
			mpq_get_num (n, y.rep);
			mpq_get_den (d, y.rep);

			mpz_div (n, n, d);

			x = integer::zero;

			// Really bad, but I know of no other general way to do this
			while (mpz_sgn (n) != 0) {
				// We need to be ready for multiple word sizes and so on here...
				x = (x << (sizeof (unsigned long) << 3)) + mpz_get_ui (n);
				mpz_tdiv_q_2exp (n, n, sizeof (unsigned long) << 3);
			}

			return x;
		}
    
		/** Assignment of one field element to another.
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 *
		 * FIXME: Is this x := y? I am assuming so.
		 */
		element &assign (element &x, const element &y) const
		{
			mpq_set (x.rep, y.rep);
			return x;
		}
    
		/** Cardinality.
		 * Return integer representing cardinality of the field.
		 * Returns a non-negative integer for all fields with finite
		 * cardinality, and returns -1 to signify a field of infinite 
		 * cardinality.
		 * @return constant reference to integer representing cardinality 
		 *	       of the field
		 */
		integer &cardinality (integer &c) const 
			{ return c = _cardinality; }

		/** Characteristic.
		 * Return integer representing characteristic of the field.
		 * Returns a positive integer to all fields with finite characteristic,
		 * and returns 0 to signify a field of infinite characteristic.
		 * @return constant reference to integer representing characteristic 
		 * 	       of the field.
		 */
		integer &characteristic (integer &c) const
			{ return c = _characteristic; }

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
		 * In this implementation, this means for both x and y, 
		 * _elem_ptr exists and does not point to null.
		 * @return boolean true if equal, false if not.
		 * @param  x field element
		 * @param  y field element
		 */
		bool areEqual (const element &x, const element &y) const
			{ return mpq_equal (x.rep, y.rep); }

		/** Addition.
		 * x = y + z
		 * This function assumes all the field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for x, y, and z, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		element &add (element &x, const element &y, const element &z) const
		{
			mpq_add (x.rep, y.rep, z.rep);
			return x;
		}
    
		/** Subtraction.
		 * x = y - z
		 * This function assumes all the field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for x, y, and z, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		element &sub (element &x, const element &y, const element &z) const
		{
			mpq_sub (x.rep, y.rep, z.rep);
			return x;
		}
    
		/** Multiplication.
		 * x = y * z
		 * This function assumes all the field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for x, y, and z, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		element &mul (element &x, const element &y, const element &z) const
		{
			mpq_mul (x.rep, y.rep, z.rep);
			return x;
		}
    
		/** Division.
		 * x = y / z
		 * This function assumes all the field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for x, y, and z, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 * @param  z field element.
		 */
		element &div (element &x, const element &y, const element &z) const
		{
			mpq_div (x.rep, y.rep, z.rep);
			return x;
		}

		/** Additive Inverse (Negation).
		 * x = - y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &neg (element &x, const element &y) const
		{
			mpq_neg (x.rep, y.rep);
			return x;
		}

		/** Multiplicative Inverse.
		 * x = 1 / y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &inv (element &x, const element &y) const
		{
			mpq_inv (x.rep, y.rep);
			return x;
		}

		//@} Arithmetic Operations

		/** @name Inplace Arithmetic Operations 
		 * x <- x op y; x <- op x
		 * These operations require all elements, including x, to be initialized
		 * before the operation is called.  Uninitialized field elements will
		 * give undefined results.
		 */
		//@{
    
		/** Zero equality.
		 * Test if field element is equal to zero.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means the _elem_ptr of x
		 * exists and does not point to null.
		 * @return boolean true if equals zero, false if not.
		 * @param  x field element.
		 */
		bool isZero (const element &x) const 
			{ return mpq_sgn (x.rep) == 0; }
    
		/** One equality.
		 * Test if field element is equal to one.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means the _elem_ptr of x
		 * exists and does not point to null.
		 * @return boolean true if equals one, false if not.
		 * @param  x field element.
		 */
		bool isOne (const element &x) const 
			{ return mpq_cmp_ui (x.rep, 1L, 1L) == 0; }
    
		/** Inplace Addition.
		 * x += y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &addin (element &x, const element &y) const
		{
			mpq_add (x.rep, x.rep, y.rep);
			return x;
		}

		/** Inplace Subtraction.
		 * x -= y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &subin (element &x, const element &y) const
		{
			mpq_sub (x.rep, x.rep, y.rep);
			return x;
		}
 
		/** Inplace Multiplication.
		 * x *= y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &mulin (element &x, const element &y) const
		{
			mpq_mul (x.rep, x.rep, y.rep);
			return x;
		}

		element &axpy (element &r, const element &a, const element &x, const element &y) const
		{
			mpq_mul (r.rep, a.rep, x.rep);
			mpq_add (r.rep, r.rep, y.rep);
			return r;
		}

		element &axpyin (element &r, const element &a, const element &x) const
		{
			element tmp;
			mpq_mul (tmp.rep, a.rep, x.rep);
			mpq_add (r.rep, r.rep, tmp.rep);
			return r;
		}

		/** Inplace Division.
		 * x /= y
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &divin (element &x, const element &y) const
		{
			mpq_div (x.rep, x.rep, y.rep);
			return x;
		}
    
		/** Inplace Additive Inverse (Inplace Negation).
		 * x = - x
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means the _elem_ptr of x
		 * exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 */
		element &negin (element &x) const
		{
			mpq_neg (x.rep, x.rep);
			return x;
		}

		/** Inplace Multiplicative Inverse.
		 * x = 1 / x
		 * This function assumes the field elementhas already been 
		 * constructed and initialized.
		 * In this implementation, this means the _elem_ptr of x
		 * exists and does not point to null.
		 * @return reference to x.
		 * @param  x field element (reference returned).
		 */
		element &invin (element &x) const
		{
			mpq_inv (x.rep, x.rep);
			return x;
		}
    
		//@} Inplace Arithmetic Operations

		/** @name Input/Output Operations */
		//@{
    
		/** Print field.
		 * @return output stream to which field is written.
		 * @param  os  output stream to which field is written.
		 *
		 * This does not do much...
		 */
		ostream &write (ostream &os) const 
		{ 
			os << "GMP rational numbers"; 
			return os;
		}
    
		/** Read field.
		 * @return input stream from which field is read.
		 * @param  is  input stream from which field is read.
		 *
		 * This does not do much either...
		 *
		 * FIXME: Read the same thing written above, and throw an exception if the
		 * strings do not match.
		 */
		istream &read (istream &is) { return is; }
    
		/** Print field element.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means for the _elem_ptr for x 
		 * exists and does not point to null.
		 * @return output stream to which field element is written.
		 * @param  os  output stream to which field element is written.
		 * @param  x   field element.
		 */
		ostream &write (ostream &os, const element &x) const 
		{
			char *str;

			str = new char[mpz_sizeinbase (mpq_numref (x.rep), 10) + 2];
			mpz_get_str (str, 10, mpq_numref (x.rep));
			os << str;
			delete str;

			if (mpz_cmp_ui (mpq_denref (x.rep), 1L) != 0) {
				str = new char[mpz_sizeinbase (mpq_denref (x.rep), 10) + 2];
				mpz_get_str (str, 10, mpq_denref (x.rep));
				os << '/' << str;
				delete str;
			}

			return os;
		}

		/** Read field element.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means for the _elem_ptr for x 
		 * exists and does not point to null.
		 * @return input stream from which field element is read.
		 * @param  is  input stream from which field element is read.
		 * @param  x   field element.
		 *
		 * FIXME: Avoid the magical limit on size here
		 * FIXME: Right now it skips over everything until it finds something that
		 * looks like a number. Is this really the correct policy?
		 */
		istream &read (istream &is, element &x) const
		{
			char buffer[65536], endc;
			bool found_space = false;
			int i = 0;

			do {
				is.get (endc);
			} while (is && !isdigit (endc) && endc != '-' && endc != '.');

			buffer[0] = endc;

			while ((buffer[i] == '-' || isdigit (buffer[i])) && i < 65535) {
				i++;
				is.get (buffer[i]);
			}

			endc = buffer[i];
			buffer[i] = '\0';

			if (i > 0)
				mpz_set_str (mpq_numref (x.rep), buffer, 10);
			else
				mpq_set_si (x.rep, 0L, 1L);

			if (endc == ' ') {
				found_space = true;
				while (endc == ' ') is >> endc;
			}

			if (endc == '/') {
				i = 0;

				is.get (endc);
				while (isspace (endc)) is.get (endc);
				is.putback (endc);

				do {
					is.get (buffer[i++]);
				} while (isdigit (buffer[i - 1]) && i < 65536);

				is.putback (buffer[i - 1]);
				buffer[i - 1] = '\0';

				mpz_set_str (mpq_denref (x.rep), buffer, 10);
			}
			else if (endc == '.' && !found_space) {
				element decimal_part;

				mpz_set_si (mpq_denref (x.rep), 1L);
				mpq_set_si (decimal_part.rep, 1L, 1L);
				mpz_set_si (mpq_denref (decimal_part.rep), 1L);

				i = 0;

				do {
					is.get (buffer[i++]);
					if (isdigit (buffer[i - 1]))
						mpz_mul_ui (mpq_denref (decimal_part.rep),
							    mpq_denref (decimal_part.rep), 10L);
				} while (isdigit (buffer[i - 1]) && i < 65536);

				is.putback (buffer[i - 1]);
				buffer[i - 1] = '\0';

				mpz_set_str (mpq_numref (decimal_part.rep), buffer, 10);
				mpq_canonicalize (decimal_part.rep);

				mpq_add (x.rep, x.rep, decimal_part.rep);
			}
			else {
				is.putback (endc);
				mpz_set_si (mpq_denref (x.rep), 1L);
			}

			mpq_canonicalize (x.rep);

			return is;
		}

		//@} Input/Output Operations
    
		//@} Common Object Interface

		GMPRationalField ()
			: zero (_zero, _one), one (_one, _one), neg_one (_neg_one, _one)
			{}

		const element zero;
		const element one;
		const element neg_one;
    
	    private:

		static const integer _cardinality;
		static const integer _characteristic;

		static const integer _zero;
		static const integer _one;
		static const integer _neg_one;

	}; // class GMPRationalField

	ostream &operator << (ostream &os, GMPRationalElement &elt)
	{
		GMPRationalField field;

		field.write (os, elt);
		return os;
	}

	istream &operator >> (istream &is, GMPRationalElement &elt)
	{
		GMPRationalField field;

		field.read (is, elt);
		return is;
	}

} // namespace LinBox

#endif // _GMP_RATIONAL_FIELD_
