#ifndef __LINBOX_MODULAR__INT16_H
#define __LINBOX_MODULAR__INT16_H


#include "linbox-config.h"
#include "linbox/integer.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/field/field-interface.h"
#include "linbox/util/debug.h"

#ifndef LINBOX_MAX_INT16
#define LINBOX_MAX_INT16 32767
#endif

#ifndef LINBOX_MAX_INT16_MODULUS
#define LINBOX_MAX_INT16_MODULUS 32767
#endif

// Namespace in which all LinBox code resides
namespace LinBox 
{ 

	template<class Element>
		class Modular;
	
	template<class Element>
		class ModularRandIter;

	template<class Field>
		class FieldAXPY;

	template<class Field>
		class DotProductDomain;

	template<class Field>
		class MVProductDomain;

	/** @memo Specialization of Modular to short element type with efficient dot product.
	 * @doc
	 * Efficient element operations for dot product, mul, axpy, by using floating point
	 * inverse of modulus (borrowed from NTL) and some use of non-normalized intermediate values.
	 * 
	 * Requires: modulus < 2^15. 
	 * Intended use: 2^7 < prime modulus < 2^15.
	 */
	template <>
		class Modular<int16> : public FieldInterface {
		protected:
		int16 modulus;
		double modulusinv;

		public:	       
		friend class FieldAXPY<Modular<int16> >;
                friend class DotProductDomain<Modular<int16> >;
		friend class MVProductDomain<Modular<int16> >;

		typedef int16 Element;
		typedef ModularRandIter<int16> RandIter;

		//default modular field,taking 65521 as default modulus
		Modular () :modulus(251){modulusinv=1/(double)251;}

		Modular (int value)  : modulus(value) {
			modulusinv = 1 / ((double) value); 
			if(value<=1) throw PreconditionFailed(__FUNCTION__,__LINE__,"modulus must be > 1");
			if(value>LINBOX_MAX_INT16_MODULUS) throw PreconditionFailed(__FUNCTION__,__LINE__,"modulus is too big");
		}

		Modular(const Modular<int16>& mf) : modulus(mf.modulus),modulusinv(mf.modulusinv){}

		const Modular &operator=(const Modular<int16> &F) {
			modulus = F.modulus;
			modulusinv = F.modulusinv;
			return *this;
		}

	
		integer &cardinality (integer &c) const{ 
			return c = modulus;
		}

		integer &characteristic (integer &c) const {
			return c = modulus; 
		}

		integer &convert (integer &x, const Element &y) const { 
			return x = y;
		}
		
		std::ostream &write (std::ostream &os) const {
			return os << "int16 mod " << modulus;
		}
		
		std::istream &read (std::istream &is) {
			int prime;
			is >> prime; 
			modulus = prime;
			modulusinv = 1 /((double) modulus );
			if(prime <= 1) throw PreconditionFailed(__FUNCTION__,__LINE__,"modulus must be > 1");
			if(prime > LINBOX_MAX_INT16_MODULUS) throw PreconditionFailed(__FUNCTION__,__LINE__,"modulus is too big");
		
			return is;
		}
		
		std::ostream &write (std::ostream &os, const Element &x) const {
			return os << x;
		}

		std::istream &read (std::istream &is, Element &x) const {
			integer tmp;
			is >> tmp;
			init(x,tmp); 
			return is;
                }
		

		Element &init (Element &x, const integer &y) const  {
			x = y % integer (modulus);
			if (x < 0) x += modulus;
			return x;
		}

		inline Element& init(Element& x, int y =0) const {
			x = y % modulus;
			if ( x < 0 ) x += modulus;
			return x;
		}

		inline Element& init(Element& x, long y) const {
			x = y % modulus;
			if ( x < 0 ) x += modulus;
			return x;
		}
		
		inline Element& assign(Element& x, const Element& y) const {
			return x = y;
		}
									
		
		inline bool areEqual (const Element &x, const Element &y) const {
			return x == y;
		}

		inline  bool isZero (const Element &x) const {
			return x == 0; 
		}
		
		inline bool isOne (const Element &x) const {
			return x == 1; 
		}

		inline Element &add (Element &x, const Element &y, const Element &z) const {
			x = y + z;
			if ( (uint16)x >= (uint16)modulus ) x =( (uint16)x )- modulus;
			return x;
		}
 
		inline Element &sub (Element &x, const Element &y, const Element &z) const {
			x = y - z;
			if (x < 0) x += modulus;
			return x;
		}
		
		inline Element &mul (Element &x, const Element &y, const Element &z) const {
			int16 q;

			double ab=((double) y)* ((double) z);		
			q  = (int16)(ab*modulusinv);  // q could be off by (+/-) 1
			x = (int16) (ab - ((double) q )* ((double) modulus));
			
			
			if (x >= modulus)
				x -= modulus;
			else if (x < 0)
				x += modulus;

			return x;
		}
 
		inline Element &div (Element &x, const Element &y, const Element &z) const {
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}
 
		inline Element &neg (Element &x, const Element &y) const {
			if(y==0) return x=0;
			else return x=modulus-y;
		}
 
		inline Element &inv (Element &x, const Element &y) const {
			int16 d, t;			
			XGCD(d, x, t, y, modulus);
			if (d != 1)
				throw PreconditionFailed(__FUNCTION__,__LINE__,"InvMod: inverse undefined");
			if (x < 0)
				return x += modulus;
			else
				return x;
							      
		}

		inline Element &axpy (Element &r, 
				      const Element &a, 
				      const Element &x, 
				      const Element &y) const {
			int16 q;

			double ab=((double) a)* ((double) x) + ( double ) y;		
			q  = (int16)(ab*modulusinv);  // q could be off by (+/-) 1
			r = (int16) (ab - ((double) q )* ((double) modulus));
			
			
			if (r >= modulus)
				r -= modulus;
			else if (r < 0)
				r += modulus;

			return r;

		}

		inline Element &addin (Element &x, const Element &y) const {
			x += y;
			if ( ((uint16) x) >= (uint16)modulus ) x = ((uint16) x)-modulus;
			return x;
		}
 
		inline Element &subin (Element &x, const Element &y) const {
			x -= y;
			if (x < 0) x += modulus;
			return x;
		}
 
		inline Element &mulin (Element &x, const Element &y) const {
			return mul(x,x,y);
		}
 
		inline Element &divin (Element &x, const Element &y) const {
			return div(x,x,y);
		}
 
		inline Element &negin (Element &x) const {
			if (x == 0) return x; 
			else return x = modulus - x; 
		}
 
		inline Element &invin (Element &x) const {
			return inv (x, x);
		}

		inline Element &axpyin (Element &r, const Element &a, const Element &x) const {
			
			int16 q;

			double ab = ((double) a)* ((double) x) + ( double ) r;		
			q  = (int16)(ab*modulusinv);  // q could be off by (+/-) 1
			r = (int16) (ab - ((double) q )* ((double) modulus));
			
			
			if (r >= modulus)
				r -= modulus;
			else if (r < 0)
				r += modulus;

			return r;
		}

		private:

      		static void XGCD(int16& d, int16& s, int16& t, int16 a, int16 b) {
			int16  u, v, u0, v0, u1, v1, u2, v2, q, r;
			
			int16 aneg = 0, bneg = 0;
			
			if (a < 0) {
				if (a < -LINBOX_MAX_INT16) throw PreconditionFailed(__FUNCTION__,__LINE__,"XGCD: integer overflow");
				a = -a;
				aneg = 1;
			}
			
			if (b < 0) {
				if (b < -LINBOX_MAX_INT16) throw PreconditionFailed(__FUNCTION__,__LINE__,"XGCD: integer overflow");
				b = -b;
				bneg = 1;
			}
			
			u1 = 1; v1 = 0;
			u2 = 0; v2 = 1;
			u = a; v = b;
			
			while (v != 0) {
				q = u / v;
				r = u % v;
				u = v;
				v = r;
				u0 = u2;
				v0 = v2;
				u2 =  u1 - q*u2;
				v2 = v1- q*v2;
				u1 = u0;
				v1 = v0;
			}
			
			if (aneg)
				u1 = -u1;
			
			if (bneg)
				v1 = -v1;
			
			d = u;
			s = u1;
			t = v1;
		}
		
	};

	template <>
		class FieldAXPY<Modular<int16> > {	  
		public:
	  
		typedef int16 Element;
		typedef Modular<int16> Field;
	  
		FieldAXPY (const Field &F) : _F (F),_y(0) {
		}

		FieldAXPY (const FieldAXPY &faxpy) : _F (faxpy._F), _y (0){}
	  
		FieldAXPY<Modular<int16> > &operator = (const FieldAXPY &faxpy) {
			_F = faxpy._F; 
			_y = faxpy._y; 
			return *this; 
		}
	  
		inline void accumulate (const Element &a, const Element &x) {
			uint64 t = ( (uint32) a ) * ( (uint32) x );
			_y += t;		 
		}

		inline Element& get (Element &y) {
			y = _y % (uint64) _F.modulus;
			return y;
		}

		inline FieldAXPY &assign (const Element y) {
			_y = y; 
			return *this;
		}

		inline void reset() {
			_y = 0;
		}

		private:
	  
		Field _F;
		uint64 _y;
		uint16 _two_64;
	};


	template <>
		class DotProductDomain<Modular<int16> > : private virtual VectorDomainBase<Modular<int16> > {

		public:	  
		typedef int16 Element;	  
		DotProductDomain (const Modular<int16> &F)
			: VectorDomainBase<Modular<int16> > (F) {
		}
	  
	  
		protected:
		template <class Vector1, class Vector2>
			inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const {
		  
			typename Vector1::const_iterator i;
			typename Vector2::const_iterator j;
		  
			uint64 y = 0;
			uint64 t;
		  
			for (i = v1.begin (), j = v2.begin (); i < v1.end (); ++i, ++j) {
				y  += ( (uint32) *i ) * ( (uint32) *j );
			}
		       
			y %= (uint64) _F.modulus;
		  
			return res = y;
			
		}
	  
		template <class Vector1, class Vector2>
			inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const {		  
			typename Vector1::first_type::const_iterator i_idx;
			typename Vector1::second_type::const_iterator i_elt;
		  
			uint64 y = 0;
		  
			for (i_idx = v1.first.begin (), i_elt = v1.second.begin (); i_idx != v1.first.end (); ++i_idx, ++i_elt) {
				y += ( (uint32) *i_elt ) * ( (uint32) v2[*i_idx] );
			}
		
			y %= (uint64) _F.modulus;
		  
			return res = y;
		}
	  
	};
	// Specialization of MVProductDomain for int16 modular field	

	template <>
		class MVProductDomain<Modular<int16> >
		{
		public:

			typedef int16 Element;

		protected:
			template <class Vector1, class Matrix, class Vector2>
				inline Vector1 &mulColDense
				(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v) const
				{
					return mulColDenseSpecialized
						(VD, w, A, v, VectorTraits<typename Matrix::Column>::VectorCategory ());
				}

		private:
			template <class Vector1, class Matrix, class Vector2, class RowTrait>
				Vector1 &mulColDenseSpecialized
				(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
				 VectorCategories::DenseVectorTag<RowTrait>) const;
			template <class Vector1, class Matrix, class Vector2, class RowTrait>
				Vector1 &mulColDenseSpecialized
				(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
				 VectorCategories::SparseSequenceVectorTag<RowTrait>) const;
			template <class Vector1, class Matrix, class Vector2, class RowTrait>
				Vector1 &mulColDenseSpecialized
				(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
				 VectorCategories::SparseAssociativeVectorTag<RowTrait>) const;
			template <class Vector1, class Matrix, class Vector2, class RowTrait>
				Vector1 &mulColDenseSpecialized
				(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
				 VectorCategories::SparseParallelVectorTag<RowTrait>) const;

			mutable std::vector<uint64> _tmp;
		};

	template <class Vector1, class Matrix, class Vector2, class RowTrait>
		Vector1 &MVProductDomain<Modular<int16> >::mulColDenseSpecialized
		(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::DenseVectorTag<RowTrait>) const {
		
		linbox_check (A.coldim () == v.size ());
		linbox_check (A.rowdim () == w.size ());
		
		typename Matrix::ConstColIterator i = A.colBegin ();
		typename Vector2::const_iterator j;
		typename Matrix::Column::const_iterator k;
		std::vector<uint64>::iterator l;

		uint64 t;

		if (_tmp.size () < w.size ())
			_tmp.resize (w.size ());
		
		std::fill (_tmp.begin (), _tmp.begin () + w.size (), 0);
		
		for (j = v.begin (); j != v.end (); ++j, ++i) {
			for (k = i->begin (), l = _tmp.begin (); k != i->end (); ++k, ++l) {
				t = ((uint32) *k) * ((uint32) *j);

				*l += t;
				
			}
		}
		
		typename Vector1::iterator w_j;
		
		for (w_j = w.begin (), l = _tmp.begin (); w_j != w.end (); ++w_j, ++l)
			*w_j = *l % VD.field ().modulus;
		
		return w;
	}
	
	template <class Vector1, class Matrix, class Vector2, class RowTrait>
		Vector1 &MVProductDomain<Modular<int16> >::mulColDenseSpecialized
		(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseSequenceVectorTag<RowTrait>) const
		{
			linbox_check (A.coldim () == v.size ());
			linbox_check (A.rowdim () == w.size ());
			
			typename Matrix::ConstColIterator i = A.colBegin ();
			typename Vector2::const_iterator j;
			typename Matrix::Column::const_iterator k;
			std::vector<uint64>::iterator l;
			
			uint64 t;
			
			if (_tmp.size () < w.size ())
				_tmp.resize (w.size ());
			
			std::fill (_tmp.begin (), _tmp.begin () + w.size (), 0);
			
			for (j = v.begin (); j != v.end (); ++j, ++i) {
				for (k = i->begin (), l = _tmp.begin (); k != i->end (); ++k, ++l) {
					t = ((uint32) k->second) * ((uint32) *j);

					_tmp[k->first] += t;
					
				}
			}
			
			typename Vector1::iterator w_j;
			
			for (w_j = w.begin (), l = _tmp.begin (); w_j != w.end (); ++w_j, ++l)
				*w_j = *l % VD.field ().modulus;
			
			return w;
		}
	
	template <class Vector1, class Matrix, class Vector2, class RowTrait>
		Vector1 &MVProductDomain<Modular<int16> >::mulColDenseSpecialized
		(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseAssociativeVectorTag<RowTrait>) const {

		linbox_check (A.coldim () == v.size ());
		linbox_check (A.rowdim () == w.size ());
		
		typename Matrix::ConstColIterator i = A.colBegin ();
		typename Vector2::const_iterator j;
		typename Matrix::Column::const_iterator k;
		std::vector<uint64>::iterator l;
		
		uint64 t;
		
		if (_tmp.size () < w.size ())
			_tmp.resize (w.size ());
		
		std::fill (_tmp.begin (), _tmp.begin () + w.size (), 0);
		
		for (j = v.begin (); j != v.end (); ++j, ++i) {
			for (k = i->begin (), l = _tmp.begin (); k != i->end (); ++k, ++l) {
				t = ((uint32) k->second) * ((uint32) *j);
				
				_tmp[k->first] += t;
				
			}
		}
		
		typename Vector1::iterator w_j;
		
		for (w_j = w.begin (), l = _tmp.begin (); w_j != w.end (); ++w_j, ++l)
			*w_j = *l % VD.field ().modulus;
		
		return w;
	}

	template <class Vector1, class Matrix, class Vector2, class RowTrait>
		Vector1 &MVProductDomain<Modular<int16> >::mulColDenseSpecialized
		(const VectorDomain<Modular<int16> > &VD, Vector1 &w, const Matrix &A, const Vector2 &v,
		 VectorCategories::SparseParallelVectorTag<RowTrait>) const {
		
		linbox_check (A.coldim () == v.size ());
		linbox_check (A.rowdim () == w.size ());
		
		typename Matrix::ConstColIterator i = A.colBegin ();
		typename Vector2::const_iterator j;
		typename Matrix::Column::first_type::const_iterator k_idx;
		typename Matrix::Column::second_type::const_iterator k_elt;
		std::vector<uint64>::iterator l;
		
		uint64 t;
		
		if (_tmp.size () < w.size ())
			_tmp.resize (w.size ());
		
		std::fill (_tmp.begin (), _tmp.begin () + w.size (), 0);
		
		for (j = v.begin (); j != v.end (); ++j, ++i) {
			for (k_idx = i->first.begin (), k_elt = i->second.begin (), l = _tmp.begin ();
			     k_idx != i->first.end ();
			     ++k_idx, ++k_elt, ++l)
				{
					t = ((uint32) *k_elt) * ((uint32) *j);

					_tmp[*k_idx] += t;

				}
		}

		typename Vector1::iterator w_j;

		for (w_j = w.begin (), l = _tmp.begin (); w_j != w.end (); ++w_j, ++l)
			*w_j = *l % VD.field ().modulus;

		return w;
	}
  	  

} 

#include "linbox/randiter/modular.h"
#endif
