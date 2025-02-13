/* Copyright (C) LinBox
 *  Author: Zhendong Wan
 *
 *
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */

/** @file field/ntl/ntl-ZZ.h
 * @ingroup field
 * @brief NO DOC
 */

#ifndef __LINBOX_ntl_zz_H
#define __LINBOX_ntl_zz_H

#include <NTL/ZZ.h>
#include "linbox/integer.h"
#include <iostream>
#include "linbox/util/debug.h"
#include "linbox/randiter/ntl-zz.h"
#include "linbox/field/field-traits.h"

namespace Givaro
{
	/** Initialization of field element from an integer.
	 * Behaves like C++ allocator construct.
	 * This function assumes the output field element x has already been
	 * constructed, but that it is not already initialized.
	 * For now, this is done by converting the integer type to a C++
	 * long and then to the element type through the use of static cast and
	 * NTL's to_ZZ function.
	 * This, of course, assumes such static casts are possible.
	 * This function should be changed in the future to avoid using long.
	 * @return reference to field element.
	 * @param x field element to contain output (reference returned).
	 * @param y integer.
	 */
	template <>
	inline NTL::ZZ& Caster(NTL::ZZ& x, const Integer& y)
	{
		std::stringstream s;
		s << y;
		s >> x;
		return x;
	}
	template <> inline NTL::ZZ& Caster(NTL::ZZ& x, const double& y) { return x = NTL::to_ZZ((long)(y)); }

	template <> inline NTL::ZZ& Caster(NTL::ZZ& x, const int32_t& y) { return x = NTL::to_ZZ((long)(y)); }

	template <> inline NTL::ZZ& Caster(NTL::ZZ& x, const int64_t& y) { return x = NTL::to_ZZ((long)(y)); }

	template <> inline NTL::ZZ& Caster(NTL::ZZ& x, const uint32_t& y) { return x = NTL::to_ZZ((unsigned long)(y)); }

	template <> inline NTL::ZZ& Caster(NTL::ZZ& x, const uint64_t& y) { return x = NTL::to_ZZ((unsigned long)(y)); }

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
	inline Integer& Caster(Integer& x, const NTL::ZZ& y)
	{
		std::stringstream s;
		s << y;
		s >> x;
		return x;
		//return x = static_cast<Integer>(to_long(y));
	}
} // namespace Givaro

namespace LinBox
{

	template <class Ring>
	struct ClassifyRing ;

	class NTL_ZZ;

	template <>
	struct ClassifyRing<NTL_ZZ>
	{
		typedef RingCategories::IntegerTag categoryTag;
	};

	template<class Field>
	class FieldAXPY;

	/// \brief the integer ring. \ingroup ring
	class NTL_ZZ {

	public:
		typedef NTL_ZZRandIter RandIter;
		typedef NTL_ZZ Father_t ;

		typedef NTL::ZZ Element;
		typedef NTL::ZZ Residu_t;
		Element zero,one,mOne;

		NTL_ZZ(int p = 0, int exp = 1)
		{
			if( p != 0 ) throw PreconditionFailed(LB_FILE_LOC,"modulus must be 0 (no modulus)");
			if( exp != 1 ) throw PreconditionFailed(LB_FILE_LOC,"exponent must be 1");
		}

		inline integer& cardinality (integer& c) const
		{
			return c = -1;
		}

		inline integer& characteristic (integer& c)const
		{
			return c = 0;
		}

		template<class IntType>
		inline IntType & characteristic (IntType& c)const
		{
			return c = 0;
		}

		std::ostream& write (std::ostream& out) const
		{
			return out << "NTL ZZ Ring";
		}

		std::istream& read (std::istream& in) const
		{
			return in;
		}

		/** @brief
		 *  Init x from y.
		 */
		template<class Element2>
		inline Element& init (Element& x,  const Element2& y) const
		{

			NTL::conv (x, y);

			return x;
		}

		/** @brief
		 *   Init from a NTL::ZZ
		 */
		inline Element& init (Element& x, const Element& y) const
		{

			x = y;

			return x;
		}

		/** @brief
		 *   Init from an int64_t
		 */
		inline Element& init (Element& x, const int64_t& y) const
		{
			bool isNeg = false;
			uint64_t t;
			if( y < 0 ) {
				isNeg = true;
				t = uint64_t(y * -1);
			}
			else t = (uint64_t) y;
			init(x,t);
			if( isNeg ) x *= -1;
			return x;
		}

		/** @brief
		 *   Init from a uint64_t
		 */
		inline Element& init (Element& x, const uint64_t& y) const
		{
			uint64_t shift = (uint64_t)1 << 32;
			uint32_t temp = uint32_t(y % shift);
			NTL::conv (x,temp);
			x <<= 32;
			temp = (uint32_t)(y / shift);
			x += temp;
			return x;
		}

		/** @brief
		 *  I don't  know how to init from integer efficiently.
		 */
		// c_str is safer than data, Z. W and BDS
		inline Element& init (Element& x, const integer& y) const
		{

			return x=NTL::to_ZZ((std::string(y)).c_str());
		}

		/** @brief
		 *  Convert y to an Element.
		 */
		inline integer& convert (integer& x, const Element& y) const
		{
			bool Neg=false;
			if (sign(y) <0)
				Neg=true;
			long b = NumBytes(y);
			unsigned char* byteArray;
			byteArray = new unsigned char[(size_t)b ];
			BytesFromZZ(byteArray, y, b);

			integer base(256);
			x= integer(0);

			for(long i = b - 1; i >= 0; --i) {
				x *= base;
				x += integer(byteArray[i]);
			}
			delete [] byteArray;
			if (Neg)
				x=-x;
			return x;
		}

		inline double& convert (double& x, const Element& y) const
		{
			return x=NTL::to_double(y);
		}



		/** @brief
		 *  x = y.
		 */
		inline Element&  assign (Element& x, const Element& y)  const
		{
			return x = y;
		}

		/** @brief
		 *  Test if x == y
		 */
		inline bool areEqual (const Element& x ,const Element& y) const
		{
			return x == y;
		}

		/** @brief
		 *  Test if x == 0
		 */
		inline bool isZero (const Element& x) const
		{
			return NTL::IsZero (x);
		}

		/** @brief
		 *  Test if x == 1
		 */
		inline bool isOne (const Element& x) const
		{
			return NTL::IsOne (x);
		}
		/** @brief
		 *  Test if x == -1
		 */
		inline bool isMOne (const Element& x) const
		{
			Element y ; neg(y,x);
			return isOne(y);

		}

		// arithmetic

		/** @brief
		 *  return x = y + z
		 */
		inline Element& add (Element& x, const Element& y, const Element& z) const
		{

			NTL::add (x, y, z);

			return x;
		}

		/** @brief
		 *  return x = y - z
		 */
		inline Element& sub (Element& x, const Element& y, const Element& z) const
		{

			NTL::sub (x, y, z);

			return x;
		}

		/** @brief
		 *  return x = y * z
		 */
		template <class Int>
		inline Element& mul (Element& x, const Element& y, const Int& z) const
		{

			NTL::mul (x, y, z);

			return x;
		}

		/** @brief
		 *  If z divides y, return x = y / z,
		 *  otherwise, throw an exception
		 */
		inline Element& div (Element& x, const Element& y, const Element& z) const
		{

			Element q, r;

			NTL::DivRem (q, r, y, z);

			if (NTL::IsZero (r))
				return x = q;

			else
				throw PreconditionFailed(LB_FILE_LOC,"Div: not dividable");
		}

		/** @brief
		 *  If y is a unit, return x = 1 / y,
		 *  otherwsie, throw an exception
		 */
		inline Element& inv (Element& x, const Element& y) const
		{

			if ( NTL::IsOne (y)) return x = y;

			else if ( NTL::IsOne (-y)) return x = y;

			else
				throw PreconditionFailed(LB_FILE_LOC,"Inv: Not invertible");
		}

		/** @brief
		 *  return x = -y;
		 */
		inline Element& neg (Element& x, const Element& y) const
		{

			NTL::negate (x, y);

			return x;
		}


		/** @brief
		 *  return r = a x + y
		 */
		template <class Int>
		inline Element& axpy (Element& r, const Element& a, const Int& x, const Element& y) const
		{

			NTL::mul (r, a, x);

			return r += y;
		}


		// inplace operator

		/** @brief
		 *  return x += y;
		 */
		inline Element& addin (Element& x, const Element& y) const
		{

			return x += y;
		}

		/** @brief
		 *  return x -= y;
		 */
		inline Element& subin (Element& x, const Element& y)  const
		{

			return x -= y;
		}

		/** @brief
		 *  return x *= y;
		 */
		template<class Int>
		inline Element& mulin (Element& x, const Int& y)  const
		{

			return x *= y;
		}

		/** @brief
		 *  If y divides x, return x /= y,
		 *  otherwise throw an exception
		 */
		inline Element& divin (Element& x, const Element& y) const
		{

			div (x, x, y);

			return x;
		}

		/** @brief
		 *  If x is a unit, x = 1 / x,
		 *  otherwise, throw an exception.
		 */
		inline Element& invin (Element& x) {

			if (NTL::IsOne (x)) return x;

			else if (NTL::IsOne (-x)) return x;

			else throw PreconditionFailed(LB_FILE_LOC,"Div: not dividable");
		}

		/** @brief
		 *  return x = -x;
		 */
		inline Element& negin (Element& x) const
		{

			NTL::negate (x, x);

			return x;
		}

		/** @brief
		 *  return r += a x
		 */
		template <class Int>
		inline Element& axpyin (Element& r, const Element& a, const Int& x) const
		{

			return r += a * x;
		}


		// IO

		/** @brief
		 *  out << y;
		 */
		std::ostream& write(std::ostream& out,const Element& y) const
		{

			out << y;

			return out;
		}


		/** @brief
		 *  read x from istream in
		 */
		std::istream& read(std::istream& in, Element& x) const
		{

			return in >> x;
		}


		/** some PIR function
		*/

		/** @brief
		 *  Test if x is a unit.
		 */
		bool isUnit(const Element& x) const
		{
			return isOne(x) || isMOne(x);
		}

		/** @brief
		 *  return g = gcd (a, b)
		 */
		inline Element& gcd (Element& g, const Element& a, const Element& b) const
		{

			NTL::GCD (g, a, b);

			return g;
		}

		/** @brief
		 *  return g = gcd (g, b)
		 */
		inline Element& gcdin (Element& g, const Element& b) const
		{

			NTL::GCD (g, g, b);

			return g;
		}

		/** @brief
		 *  g = gcd(a, b) = a*s + b*t.
		 *  The coefficients s and t are defined according to the standard
		 *  Euclidean algorithm applied to |a| and |b|, with the signs then
		 *  adjusted according to the signs of a and b.
		 */
		inline Element& xgcd (Element& g, Element& s, Element& t, const Element& a, const Element& b)const
		{

			NTL::XGCD (g,s,t,a,b);

			return g;
		}

		/** @brief
		 *  c = lcm (a, b)
		 */
		inline Element& lcm (Element& c, const Element& a, const Element& b) const
		{


			if (NTL::IsZero (a) || NTL::IsZero (b)) return c = NTL::ZZ::zero();

			else {
				Element g;

				NTL::GCD (g, a, b);

				NTL::mul (c, a, b);

				c /= g;

				NTL::abs (c, c);

				return c;
			}
		}

		/** @brief
		 *  l = lcm (l, b)
		 */
		inline Element& lcmin (Element& l, const Element& b) const
		{

			if (NTL::IsZero (l) || NTL::IsZero (b))

				return l = NTL::ZZ::zero();

			else {

				Element g;

				NTL::GCD (g, l, b);

				l *= b;

				l /= g;

				NTL::abs (l, l);

				return l;
			}
		}





		// some specail function

		/** @brief
		 *  x = floor ( sqrt(y)).
		 */

		inline Element& sqrt (Element& x, const Element& y) const
		{

			NTL::SqrRoot(x,y);

			return x;
		}

		/** @brief
		 *  Requires 0 <= x < m, m > 2 * a_bound * b_bound,
		 *  a_bound >= 0, b_bound > 0
		 *   This routine either returns 0, leaving a and b unchanged,
		 *   or returns 1 and sets a and b so that
		 *  (1) a = b x (mod m),
		 *  (2) |a| <= a_bound, 0 < b <= b_bound, and
		 *  (3) gcd(m, b) = gcd(a, b).
		 */

		inline long RationalReconstruction (Element& a, Element& b, const Element& x, const Element& m,
						 const Element& a_bound, const Element& b_bound) const
		{

			return NTL::ReconstructRational(a,b,x,m,a_bound,b_bound);
		}


		/** @brief
		 *  q = floor (x/y);
		 */
		inline Element& quo (Element& q, const Element& a, const Element& b) const
		{

			NTL::div (q, a, b);

			return q;
		}

		/** @brief
		 *  r = remindar of  a / b
		 */
		inline Element& rem (Element& r, const Element& a, const Element& b) const
		{

			NTL::rem (r, a, b);

			return r;
		}

		/** @brief
		 *  a = quotient (a, b)
		 */
		inline Element& quoin (Element& a, const Element& b) const
		{

			return a /= b;

		}

		/** @brief
		 *  a = quotient (a, b)
		 */
		inline Element& remin (Element& x, const Element& y)  const
		{
			return x %= y;
		}


		/** @brief
		 * q = [a/b], r = a - b*q
		 * |r| < |b|, and if r != 0, sign(r) = sign(b)
		 */
		inline void quoRem (Element& q, Element& r, const Element& a, const Element& b) const
		{

			NTL::DivRem(q,r,a,b);
		}

		/** @brief
		 *  Test if b | a.
		 */
		inline bool isDivisor (const Element& a, const Element& b) const
		{

			if ( NTL::IsZero (a) ) return true;

			else if (NTL::IsZero (b)) return false;

			else {
				Element r;

				NTL::rem (r, a, b); //weird order changed, dpritcha 2004-07-19

				return NTL::IsZero (r);
			}
		}

		/** compare two elements, a and b
		 * return 1, if a > b
		 * return 0, if a = b;
		 * return -1. if a < b
		 */
		inline long compare (const Element& a, const Element& b) const
		{

			return NTL::compare (a, b);
		}

		/** return the absolute value
		 * x = abs (a);
		 */
		inline Element& abs (Element& x, const Element& a) const
		{

			NTL::abs (x, a);

			return x;
		}


		static inline int maxCardinality()
		{
			return 0;
		} // no modulus

	};


	template<>
	class FieldAXPY<NTL_ZZ> {
	public:
		typedef NTL_ZZ Field;
		typedef Field::Element Element;

		/** Constructor.
		 * A faxpy object if constructed from a Field and a field element.
		 * Copies of this objects are stored in the faxpy object.
		 * @param F field F in which arithmetic is done
		 */
		FieldAXPY (const Field &F) :
			_field (F)
		{
			_y = 0;
		}

		/** Copy constructor.
		 * @param faxpy
		 */
		FieldAXPY (const FieldAXPY<Field> &faxpy) :
			_field (faxpy._field), _y (faxpy._y)
		{}

		/** Assignment operator
		 * @param faxpy
		 */
		FieldAXPY<Field> &operator = (const FieldAXPY &faxpy)
		{
			_y = faxpy._y;
			return *this;
		}

		/** Add a*x to y
		 * y += a*x.
		 * @param a constant reference to element a
		 * @param x constant reference to element x
		 * allow optimal multiplication, such as integer * int
		 */
		template<class Element1>
		inline Element& mulacc  (const Element &a, const Element1 &x)
		{
			return _y += a * x;
		}

		inline Element& accumulate (const Element &t)
		{
			return _y += t;
		}

		/** Add a*x to y
		 * y += a*x.
		 * @param a constant reference to element a
		 * @param x constant reference to element x
		 * allow optimal multiplication, such as integer * int
		 */
		template<class Element1>
		inline Element& mulacc (const Element1 &a, const Element &x)
		{
			return _y += a * x;
		}

		inline Element& mulacc (const Element& a, const Element& b)
		{

			return _y += a * b;
		}


		/** Retrieve y
		 *
		 * Performs the delayed modding out if necessary
		 */
		inline Element &get (Element &y) { y = _y; return y; }

		/** Assign method.
		 * Stores new field element for arithmetic.
		 * @return reference to self
		 * @param y constant reference to element a
		 */
		inline FieldAXPY &assign (const Element& y)
		{
			_y = y;
			return *this;
		}

		inline void reset()
		{
			_y = 0;
		}

	private:

		/// Field in which arithmetic is done
		/// Not sure why it must be mutable, but the compiler complains otherwise
		Field _field;

		/// Field element for arithmetic
		Element _y;

	};
}

#endif //__LINBOX_ntl_zz_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
