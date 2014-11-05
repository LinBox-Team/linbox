/* linbox/field/modular.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 * Copyright (C) 2011 LinBox
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2002-04-10 Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * LargeModular is now replace by a class Modular parameterized on the element
 * type. So, the old LargeModular is equivalent to Modular<integer>. All other
 * interface details are exactly the same.
 *
 * Renamed from large-modular.h to modular.h
 * ------------------------------------
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */

#ifndef __LINBOX_field_modular_unsigned_H
#define __LINBOX_field_modular_unsigned_H
namespace LinBox { /*  uint8_t */
	/** @brief Allows compact storage when the modulus is less than 2^8.
	 *
	 *  Requires <code>1 < modulus < 2^8</code>, normally prime.  See \ref
	 *  FieldArchetype for member specifications.
	 */
	template <>
	class Modular<uint8_t> : public FieldInterface, public ModularBase<uint8_t> {
	public:
		typedef uint8_t Element;
		typedef Element* Element_ptr ;
		typedef const Element* ConstElement_ptr;
		const Element zero,one, mOne;

		Modular () :
			zero(0),one(1),mOne(0),_k (0)
		{}
		Modular (uint32_t modulus) :
			ModularBase<Element> (modulus),
			zero(0),one(1),mOne((Element)(modulus-1)),
			_k (((uint64_t) -1LL) / ((modulus - 1) * (modulus - 1))),
			_pinv (1.0 / (double) ((Element) modulus))
		{
			linbox_check(modulus < UINT8_MAX);
		}
		Modular (const integer &modulus) :
			ModularBase<Element> ((unsigned long) modulus),
			zero(0),one(1),mOne(modulus-1),
			_k (((uint64_t) -1LL) / (((Element)modulus - 1) * ((Element)modulus - 1))),
			_pinv (1.0 / (double) ((Element) modulus))
		{
			linbox_check(modulus < UINT8_MAX);
		}

		const Modular &operator=(const Modular &F)
		{
			ModularBase<Element>::_modulus = F._modulus;
			_k = F._k;
			_pinv = F._pinv;
			F.assign(const_cast<Element&>(one),F.one);
			F.assign(const_cast<Element&>(zero),F.zero);
			F.assign(const_cast<Element&>(mOne),F.mOne);


			return *this;
		}

		Element &init (Element &x, const integer &y = 0) const
		{
			x = (Element) (abs (y) % integer (ModularBase<Element>::_modulus));
			if (y < 0)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		Element &init (Element &x, const double &y) const
		{
			double z = fmod(y, (double)_modulus);
			if (z < 0) z += (double) _modulus;
			return x = (Element) (z);
		}

		Element &init (Element &x, const long int &y ) const
		{
			x = (Element)(abs (y) % (long int) (ModularBase<Element>::_modulus));
			if (y < 0L)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		Element &init (Element &x, const int &y ) const
		{
			x = (Element)(abs (y) % (int) (ModularBase<Element>::_modulus));
			if (y < 0)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		/*! add elements
		 * @todo is it faster to use uint32 and multiple casts ?
		 */
		Element &add (Element &x, const Element &y, const Element &z) const
		{
			uint32_t t = (uint32_t) y + (uint32_t) z;
			if (t >= (uint32_t) ModularBase<Element>::_modulus)
				t -= ModularBase<Element>::_modulus;
			return x = (Element)t;
		}

		Element &sub (Element &x, const Element &y, const Element &z) const
		{
			int32_t t = (int32_t) y - (int32_t) z;
			if (t < 0)
				t += ModularBase<Element>::_modulus;
			return x =  (Element)t;
		}

		Element &mul (Element &x, const Element &y, const Element &z) const
		{
			return x = Element( ((uint32_t) y * (uint32_t) z) % (uint32_t) ModularBase<Element>::_modulus );
		}

		Element &div (Element &x, const Element &y, const Element &z) const
		{
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}

		Element &neg (Element &x, const Element &y) const
		{
			if (y == 0)
				return x = y;
			else
				return x = (Element) (ModularBase<Element>::_modulus - y);
		}

		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			int32_t x_int, y_int, q, tx, ty, temp;
			x_int = ModularBase<Element>::_modulus;
			y_int = y;
			tx = 0;
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int; y_int = x_int - q * y_int;
				x_int = temp;
				temp = ty; ty = tx - q * ty;
				tx = temp;
			}

			if (tx < 0) tx += ModularBase<Element>::_modulus;

			// now x_int = gcd (modulus,residue)
			return x = (Element) tx;
		}

		Element &axpy (Element &r,
			       const Element &a,
			       const Element &x,
			       const Element &y) const
		{
			r = Element(((uint32_t) a * (uint32_t) x + (uint32_t) y) % (uint32_t) ModularBase<Element>::_modulus) ;
			return r;
		}

		Element &addin (Element &x, const Element &y) const
		{
			uint32_t t = uint32_t((long) x + (long) y);
			if (t >= (uint32_t) ModularBase<Element>::_modulus) t -= ModularBase<Element>::_modulus;
			return x = (Element) t;
		}

		/*! subin.
		 * @todo why \c long here ?
		 */
		Element &subin (Element &x, const Element &y) const
		{
			long t = x - y;
			if (t < 0) t += ModularBase<Element>::_modulus;
			return x = (Element) t;
		}

		Element &mulin (Element &x, const Element &y) const
		{
			x = (Element)( ((uint32_t) x * (uint32_t) y) % (uint32_t) ModularBase<Element>::_modulus );
			return x;
		}

		Element &divin (Element &x, const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}

		Element &negin (Element &x) const
		{
			if (x == 0)
				return x;
			else
				return x = Element(ModularBase<Element>::_modulus - x);
		}

		Element &invin (Element &x) const
		{
			return inv (x, x);
		}

		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{
			r = (Element)( ((uint32_t) r + (uint32_t) a * (uint32_t) x) % (uint32_t) ModularBase<Element>::_modulus);
			return r;
		}

		static Element getMaxModulus()
		{
			return 64;// 2^6 (ou plus ?)
		}

		Element& next(Element &x) const
		{
			return addin(x,one);
		}

	private:

		friend class FieldAXPY<Modular<Element> >;
		friend class DotProductDomain<Modular<Element> >;
		friend class MVProductDomain<Modular<Element> >;

		// Number of times one can perform an axpy into a long long
		// before modding out is mandatory.
		uint64_t _k;

		// Inverse of modulus in floating point
		double _pinv;

	}; // class Modular<uint8_t>

	/*! Specialization of FieldAXPY for uint8_t modular field */

	template <>
	class FieldAXPY<Modular<uint8_t> > {
	public:

		typedef uint8_t Element;
		typedef uint64_t Abnormal;
		typedef Modular<uint8_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F),
			i ( (int)F._k)
		{
			_y = 0;
		}

		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field),
			_y (0),
			i ((int)faxpy.field()._k)
		{}

		FieldAXPY<Modular<uint8_t> > &operator = (const FieldAXPY &faxpy)
		{
			_field = faxpy._field;
			_y = faxpy._y;
			return *this;
		}

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint32_t t = (uint32_t) a * (uint32_t) x;

			if (!i--) {
				i = int(field()._k);
				return _y = _y % (uint32_t) field()._modulus + t;
			}
			else
				return _y += t;
		}

		inline uint64_t& accumulate (const Element &t)
		{

			if (!i--) {
				i = int( field()._k );
				return _y = _y % (uint32_t) field()._modulus + t;
			}
			else
				return _y += t;
		}

		inline Element &get (Element &y) const
		{
			const_cast<FieldAXPY<Field>*>(this)->_y %= (uint32_t) field()._modulus;
			if ((int32_t) _y < 0) const_cast<FieldAXPY<Field>*>(this)->_y += field()._modulus;
			y = (uint8_t) _y;
			const_cast<FieldAXPY<Field>*>(this)->i = int(field()._k);
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{
			_y = y;
			i = int(field()._k);
			return *this;
		}

		inline void reset()
		{
			_y = 0;
		}

		inline const Field & field() const { return *_field; }
	private:

		const Field *_field;
		uint64_t _y;
		int i;
	};

	//! Specialization of DotProductDomain for unsigned short modular field

	template <>
	class DotProductDomain<Modular<uint8_t> > : public virtual VectorDomainBase<Modular<uint8_t> > {
	public:

		typedef uint8_t Element;

		DotProductDomain(){}
		DotProductDomain (const Modular<uint8_t> &F) :
			VectorDomainBase<Modular<uint8_t> > (F)
		{}

		using VectorDomainBase<Modular<uint8_t> >::field;
	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	};

	//! Specialization of MVProductDomain for uint8_t modular field

	template <>
	class MVProductDomain<Modular<uint8_t> > {
	public:

		typedef uint8_t Element;

	protected:
		template <class Vector1, class Matrix, class Vector2>
		inline Vector1 &mulColDense
		(const VectorDomain<Modular<uint8_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v) const
		{
			return mulColDenseSpecialized (VD, w, A, v, typename VectorTraits<typename Matrix::Column>::VectorCategory ());
		}

	private:
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint8_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::DenseVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint8_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseSequenceVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint8_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseAssociativeVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint8_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseParallelVectorTag) const;

		mutable std::vector<uint32_t> _tmp;
	};

}

namespace LinBox { /*  uint16_t */
	/** @brief Specialization of class Modular for uint16_t element type */
	template <>
	class Modular<uint16_t> : public FieldInterface, public ModularBase<uint16_t> {
	public:

		typedef uint16_t Element;
		typedef Element* Element_ptr ;
		typedef const Element* ConstElement_ptr;

		const Element zero,one, mOne;

		Modular () :
			zero(0),one(1),mOne(0),_k (0)
		{}
		Modular (uint32_t modulus) :
			ModularBase<Element> (modulus),
			zero(0),one(1),mOne((Element)(modulus-1)),
			_k (((uint64_t) -1LL) / ((ModularBase<Element>::_modulus - 1) * (ModularBase<Element>::_modulus - 1))),
			_pinv (1.0 / (double) ((Element) ModularBase<Element>::_modulus))
		{
			linbox_check(modulus<UINT16_MAX);
		}
		Modular (const integer &modulus) :
			ModularBase<Element> ((unsigned long) modulus),
			zero(0),one(1),mOne(Element(modulus-1)),
			_k (((uint64_t) -1LL) / ((ModularBase<Element>::_modulus - 1) * (ModularBase<Element>::_modulus - 1))),
			_pinv (1.0 / (double) ((Element) ModularBase<Element>::_modulus))
		{
			linbox_check(modulus<UINT16_MAX);
		}

		const Modular &operator=(const Modular &F)
		{
			ModularBase<Element>::_modulus = F._modulus;
			_k = F._k;
			_pinv = F._pinv;
			F.assign(const_cast<Element&>(one),F.one);
			F.assign(const_cast<Element&>(zero),F.zero);
			F.assign(const_cast<Element&>(mOne),F.mOne);
			return *this;
		}

		Element &init (Element &x, const integer &y) const
		{
			x = abs (y) % integer (ModularBase<Element>::_modulus);
			if (y < 0)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		Element &init (Element &x, const double &y) const
		{
			double z = fmod(y, (double)_modulus);
			if (z < 0)
				z += (double) _modulus;
			return x = (Element) (z);
		}

		Element &init (Element &x, const long int &y ) const
		{
			x = Element(abs (y) % (long int) (ModularBase<Element>::_modulus));
			if (y < 0)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		Element &init (Element &x, const int &y ) const
		{
			x = Element(abs (y) % (int) (ModularBase<Element>::_modulus));
			if (y < 0)
				x = Element(ModularBase<Element>::_modulus - x);
			return x;
		}

		Element &init(Element &x) const
		{
			return x = 0 ;
		}

		Element &add (Element &x, const Element &y, const Element &z) const
		{
			uint32_t t = (uint32_t) y + (uint32_t) z;
			if (t >= (uint32_t) ModularBase<Element>::_modulus)
				t -= ModularBase<Element>::_modulus;
			return x = (Element) t;
		}

		Element &sub (Element &x, const Element &y, const Element &z) const
		{
			int32_t t = (int32_t) y - (int32_t) z;
			if (t < 0) t += ModularBase<Element>::_modulus;
			return x =  (Element) t;
		}

		Element &mul (Element &x, const Element &y, const Element &z) const
		{
			return x = (Element) ( ((uint32_t) y * (uint32_t) z) % (uint32_t) ModularBase<Element>::_modulus);
		}

		Element &div (Element &x, const Element &y, const Element &z) const
		{
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}

		Element &neg (Element &x, const Element &y) const
		{
			if (y == 0)
				return x = y;
			else
				return x = (Element)(  ModularBase<Element>::_modulus - y);
		}

		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			int32_t x_int, y_int, q, tx, ty, temp;
			x_int = ModularBase<Element>::_modulus;
			y_int = y;
			tx = 0;
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int; y_int = x_int - q * y_int;
				x_int = temp;
				temp = ty; ty = tx - q * ty;
				tx = temp;
			}

			if (tx < 0) tx += ModularBase<Element>::_modulus;

			// now x_int = gcd (modulus,residue)
			return x = (Element)  tx;
		}

		Element &axpy (Element &r,
			       const Element &a,
			       const Element &x,
			       const Element &y) const
		{
			r =  (Element)( ((uint32_t) a * (uint32_t) x + (uint32_t) y) % (uint32_t) ModularBase<Element>::_modulus );
			return r;
		}

		Element &addin (Element &x, const Element &y) const
		{
			uint32_t t = uint32_t( (long) x + (long) y );
			if (t >= (uint32_t) ModularBase<Element>::_modulus) t -= ModularBase<Element>::_modulus;
			return x = (Element)  t;
		}

		Element &subin (Element &x, const Element &y) const
		{
			long t = x - y;
			if (t < 0) t += ModularBase<Element>::_modulus;
			return x =  (Element) t;
		}

		Element &mulin (Element &x, const Element &y) const
		{
			x =  (Element)( ((uint32_t) x * (uint32_t) y) % (uint32_t) ModularBase<Element>::_modulus);
			return x;
		}

		Element &divin (Element &x, const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}

		Element &negin (Element &x) const
		{
			if (x == 0)
				return x;
			else
				return x = (Element) ( ModularBase<Element>::_modulus - x);
		}

		Element &invin (Element &x) const
		{
			return inv (x, x);
		}

		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{
			r = (Element) ( ((uint32_t) r + (uint32_t) a * (uint32_t) x) % (uint32_t) ModularBase<Element>::_modulus);
			return r;
		}

		static Element getMaxModulus()
		{
			return 16384;// 2^14 (ou plus ?)
		}


	private:

		friend class FieldAXPY<Modular<Element> >;
		friend class DotProductDomain<Modular<Element> >;
		friend class MVProductDomain<Modular<Element> >;

		// Number of times one can perform an axpy into a long long
		// before modding out is mandatory.
		uint64_t _k;

		// Inverse of modulus in floating point
		double _pinv;

	}; // class Modular<uint16_t>

	/*! Specialization of FieldAXPY for uint16_t modular field */
	template <>
	class FieldAXPY<Modular<uint16_t> > {
	public:

		typedef uint16_t Element;
		typedef Modular<uint16_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F),
			i ((int)F._k)
		{ _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field), _y (0), i ((int) faxpy.field()._k)
		{}

		FieldAXPY<Modular<uint16_t> > &operator = (const FieldAXPY &faxpy)
		{
			_field = faxpy._field;
			_y = faxpy._y;
			return *this;
		}

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint64_t t = (uint64_t) ((long long) a * (long long) x);

			if (!i--) {
				i = (int) field()._k;
				return _y = _y % (uint64_t) field()._modulus + t;
			}
			else
				return _y += t;
		}

		inline uint64_t& accumulate (const Element &t)
		{
			if (!i--) {
				i = (int) field()._k;
				return _y = _y % (uint64_t) field()._modulus + t;
			}
			else
				return _y += t;
		}

		inline Element &get (Element &y) const
		{
			const_cast<FieldAXPY<Field>*>(this)->_y %= (uint64_t) field()._modulus;
			if ((int64_t) _y < 0) const_cast<FieldAXPY<Field>*>(this)->_y += field()._modulus;
			y = (uint16_t) _y;
			const_cast<FieldAXPY<Field>*>(this)->i = int(field()._k);
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{
			_y = y;
			i = (int) field()._k;
			return *this;
		}

		inline void reset()
		{
			_y = 0;
		}

		inline const Field & field() const {return *_field;}
	private:

		const Field *_field;
		uint64_t _y;
		int i;
	};

	//! Specialization of DotProductDomain for unsigned short modular field

	template <>
	class DotProductDomain<Modular<uint16_t> > : public virtual VectorDomainBase<Modular<uint16_t> > {
	public:

		typedef uint16_t Element;

		DotProductDomain () {}
		DotProductDomain (const Modular<uint16_t> &F) :
			VectorDomainBase<Modular<uint16_t> > (F)
		{}
		using VectorDomainBase<Modular<uint16_t> >::field;
		using VectorDomainBase<Modular<uint16_t> >::_field;

	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;
	};

	//! Specialization of MVProductDomain for uint16_t modular field

	template <>
	class MVProductDomain<Modular<uint16_t> > {
	public:

		typedef uint16_t Element;

	protected:
		template <class Vector1, class Matrix, class Vector2>
		inline Vector1 &mulColDense
		(const VectorDomain<Modular<uint16_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v) const
		{
			return mulColDenseSpecialized (VD, w, A, v, VectorTraits<typename Matrix::Column>::VectorCategory ());
		}

	private:
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint16_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::DenseVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint16_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseSequenceVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint16_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseAssociativeVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint16_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseParallelVectorTag) const;

		mutable std::vector<uint64_t> _tmp;
	};

}

#include <fflas-ffpack/field/modular-uint32.h>

namespace LinBox { /*  uint32_t */

	template< class Element >
	class ModularRandIter;
	template< class Field, class RandIter >
	class NonzeroRandIter;

	template<class Field>
	class DotProductDomain;
	template<class Field>
	class FieldAXPY;
	template<class Field>
	class MVProductDomain;

	template <class Ring>
	struct ClassifyRing;

	template <class Element>
	struct ClassifyRing<Modular<Element> >;

	// TODO: the rest !!!

	/** @brief Specialization of class Modular for uint32_t element type */
	template <>
	class Modular<uint32_t> : public FieldInterface
	// , public ModularBase<uint32_t>
	, public FFPACK::Modular<uint32_t> {
	public:

		typedef FFPACK::Modular<uint32_t> Father_t;

		Modular (const integer &modulus=0) :
			Father_t((uint32_t)(unsigned long) modulus)
		{
			// do checks on p
		}

		using Father_t::Element;

		typedef Modular<Element>     Self_t;
		typedef ModularRandIter<Element> RandIter;

		using Father_t::init ;

		Element &init (Element &x, const integer &y ) const
		{
			x = abs (y) % integer (_modulus);
			if (y < 0) x = _modulus - x;
			return x;
		}

		using Father_t::convert ;

		integer &convert (integer &x, const Element &y) const
		{
			return x = integer(y);
		}

		using Father_t ::cardinality ;
		integer &cardinality (integer &c) const
		{
			return c = integer(_modulus);
		}

		using Father_t ::characteristic;
		      integer &characteristic (integer &c) const
		      {
			      return c = integer(_modulus);
		      }


	private:


		friend class FieldAXPY<Modular<uint32_t> >;
		friend class DotProductDomain<Modular<uint32_t> >;
		friend class MVProductDomain<Modular<uint32_t> >;


	}; // class Modular<uint32_t>

	/*! Specialization of FieldAXPY for unsigned short modular field */

	template <>
	class FieldAXPY<Modular<uint32_t> > {
	public:

		typedef uint32_t Element;
		typedef Modular<uint32_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F), _y(0)
		{ }

		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field), _y (0)
		{}

		FieldAXPY<Modular<uint32_t> > &operator = (const FieldAXPY &faxpy)
		{
			_field = faxpy._field;
			_y = faxpy._y;
			return *this;
		}

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint64_t t = (uint64_t) a * (uint64_t) x;
			_y += t;

			if (_y < t)
				return _y += field()._two_64;
			else
				return _y;
		}

		inline uint64_t& accumulate (const Element &t)
		{
			_y += t;

			if (_y < t)
				return _y += field()._two_64;
			else
				return _y;
		}

		inline uint64_t& accumulate_special (const Element &t)
		{
			return _y += t;
		}

		inline Element &get (Element &y) const
		{
			const_cast<FieldAXPY<Field>*>(this)->_y %= (uint64_t) field()._modulus;
			//if ((int64_t) _y < 0) const_cast<FieldAXPY<Field>*>(this)->_y += field()._modulus;
			return y = (uint32_t) _y;
		}

		inline FieldAXPY &assign (const Element y)
		{
			_y = y;
			return *this;
		}

		inline void reset() {
			_y = 0;
		}

		inline const Field & field() const { return *_field; }
	private:

		const Field *_field;
		uint64_t _y;
	};

	//! Specialization of DotProductDomain for uint32_t modular field

	template <>
	class DotProductDomain<Modular<uint32_t> > : private virtual VectorDomainBase<Modular<uint32_t> > {
	public:

		typedef uint32_t Element;

		DotProductDomain () {}
		DotProductDomain (const Modular<uint32_t> &F) :
			VectorDomainBase<Modular<uint32_t> > (F)
		{}
		using VectorDomainBase<Modular<uint32_t> >::field;
		using VectorDomainBase<Modular<uint32_t> >::_field;

	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;
	};

	//! Specialization of MVProductDomain for uint32_t modular field

	template <>
	class MVProductDomain<Modular<uint32_t> > {
	public:

		typedef uint32_t Element;

	protected:
		template <class Vector1, class Matrix, class Vector2>
		inline Vector1 &mulColDense
		(const VectorDomain<Modular<uint32_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v) const
		{
			return mulColDenseSpecialized (VD, w, A, v, typename VectorTraits<typename Matrix::Column>::VectorCategory ());
		}

	private:
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint32_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::DenseVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint32_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseSequenceVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint32_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseAssociativeVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint32_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseParallelVectorTag) const;

		mutable std::vector<uint64_t> _tmp;
	};

}

#include <fflas-ffpack/field/modular-uint64.h>

namespace LinBox { /*  uint64_t */

	template< class Element >
	class ModularRandIter;
	template< class Field, class RandIter >
	class NonzeroRandIter;

	template<class Field>
	class DotProductDomain;
	template<class Field>
	class FieldAXPY;
	template<class Field>
	class MVProductDomain;

	template <class Ring>
	struct ClassifyRing;

	template <class Element>
	struct ClassifyRing<Modular<Element> >;

	// TODO: the rest !!!

	/** @brief Specialization of class Modular for uint64_t element type */
	template <>
	class Modular<uint64_t> : public FieldInterface
	// , public ModularBase<uint64_t>
	, public FFPACK::Modular<uint64_t> {
	public:

		typedef FFPACK::Modular<uint64_t> Father_t;

		Modular (const integer &modulus) :
			Father_t((uint64_t)(unsigned long) modulus)
		{
			// do checks on p
		}

		using Father_t::Element;

		typedef Modular<Element>     Self_t;
		typedef ModularRandIter<Element> RandIter;

		using Father_t::init ;

		Element &init (Element &x, const integer &y ) const
		{
			x = abs (y) % integer (_modulus);
			if (y < 0) x = _modulus - x;
			return x;
		}

		using Father_t::convert ;

		integer &convert (integer &x, const Element &y) const
		{
			return x = integer(y);
		}

		using Father_t ::cardinality ;
		integer &cardinality (integer &c) const
		{
			return c = integer(_modulus);
		}

		using Father_t ::characteristic;
		      integer &characteristic (integer &c) const
		      {
			      return c = integer(_modulus);
		      }


	private:


		friend class FieldAXPY<Modular<uint64_t> >;
		friend class DotProductDomain<Modular<uint64_t> >;
		friend class MVProductDomain<Modular<uint64_t> >;


	}; // class Modular<uint64_t>

	/*! Specialization of FieldAXPY for unsigned short modular field */

	template <>
	class FieldAXPY<Modular<uint64_t> > {
	public:

		typedef uint64_t Element;
		typedef Modular<uint64_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F), _y(0)
		{ }

		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field), _y (0)
		{}

		FieldAXPY<Modular<uint64_t> > &operator = (const FieldAXPY &faxpy)
		{
			_field = faxpy._field;
			_y = faxpy._y;
			return *this;
		}

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint64_t t = (uint64_t) a * (uint64_t) x;
			_y += t;

			if (_y < t)
				return _y += field()._two_64;
			else
				return _y;
		}

		inline uint64_t& accumulate (const Element &t)
		{
			_y += t;

			if (_y < t)
				return _y += field()._two_64;
			else
				return _y;
		}

		inline uint64_t& accumulate_special (const Element &t)
		{
			return _y += t;
		}

		inline Element &get (Element &y) const
		{
			const_cast<FieldAXPY<Field>*>(this)->_y %= (uint64_t) field()._modulus;
			//if ((int64_t) _y < 0) const_cast<FieldAXPY<Field>*>(this)->_y += field()._modulus;
			return y = (uint64_t) _y;
		}

		inline FieldAXPY &assign (const Element y)
		{
			_y = y;
			return *this;
		}

		inline void reset() {
			_y = 0;
		}

		inline const Field & field() const { return *_field; }
	private:

		const Field *_field;
		uint64_t _y;
	};

	//! Specialization of DotProductDomain for uint64_t modular field

	template <>
	class DotProductDomain<Modular<uint64_t> > : private virtual VectorDomainBase<Modular<uint64_t> > {
	public:

		typedef uint64_t Element;

		DotProductDomain () {}
		DotProductDomain (const Modular<uint64_t> &F) :
			VectorDomainBase<Modular<uint64_t> > (F)
		{}
		using VectorDomainBase<Modular<uint64_t> >::field;
		using VectorDomainBase<Modular<uint64_t> >::_field;

	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;
	};

	//! Specialization of MVProductDomain for uint64_t modular field

	template <>
	class MVProductDomain<Modular<uint64_t> > {
	public:

		typedef uint64_t Element;

	protected:
		template <class Vector1, class Matrix, class Vector2>
		inline Vector1 &mulColDense
		(const VectorDomain<Modular<uint64_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v) const
		{
			return mulColDenseSpecialized (VD, w, A, v, typename VectorTraits<typename Matrix::Column>::VectorCategory ());
		}

	private:
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint64_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::DenseVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint64_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseSequenceVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint64_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseAssociativeVectorTag) const;
		template <class Vector1, class Matrix, class Vector2>
		Vector1 &mulColDenseSpecialized
		(const VectorDomain<Modular<uint64_t> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseParallelVectorTag) const;

		mutable std::vector<uint64_t> _tmp;
	};

}


#include "linbox/field/Modular/modular.inl"
#endif // __LINBOX_field_modular_unsigned_H

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
