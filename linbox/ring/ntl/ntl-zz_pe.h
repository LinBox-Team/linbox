/* Copyright (C) LinBox
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

/*! @file field/ntl/ntl-ZZ_pE.h
 * @ingroup field
 * @ingroup NTL
 * @brief NO DOC
 */

#ifndef __LINBOX_field_ntl_zz_pe_H
#define __LINBOX_field_ntl_zz_pe_H

#ifndef __LINBOX_HAVE_NTL
#error "you need NTL here"
#endif

#include <time.h>
#include "linbox/linbox-config.h"
#include "linbox/util/debug.h"
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/ZZ_pE.h>
#include <NTL/ZZ.h>

#include <givaro/zring.h>
#include "linbox/field/field-traits.h"


#include "linbox/integer.h"
#include "ntl-zz.h"

namespace Givaro
{

	template<>
	NTL::ZZ_pE& Caster(NTL::ZZ_pE &x, const Integer &y)
	{
		std::stringstream s;
		s << y;
		s >> x;
		//x=NTL::to_ZZ_pE(static_cast<long>(y));
		return x;
	}
	template<>
	NTL::ZZ_pE& Caster(NTL::ZZ_pE &x, const double &y)
	{
		std::stringstream s;
		s << y;
		s >> x;
		//x=NTL::to_ZZ_pE(static_cast<long>(y));
		return x;
	}

	// Rich Seagraves, 7-15-03
	// On the orders of Dr Saunders, I'm re-writing init & convert so that
	// they convert a ZZpE into a padic number, ie a0 + a1x + a2x^2 +... ->
	// a0 + a1*p + a2*p^2 + ...
	//

	Integer& Caster(Integer& c, const NTL::ZZ_pE& e)
	{
		NTL::ZZ_pX poly = rep(e);
		Integer base, coef;
		std::stringstream s;
		s << NTL::ZZ_p::modulus();
		s >> base;
		//Integer base = static_cast<Integer>(to_long(NTL::ZZ_p::modulus()));
		long i;

		c = 0;
		for(i = deg(poly); i >= 0; --i) {
			c *= base;
			s.clear();
			s << rep(coeff(poly, i));
			s >> coef;
			c +=  coef; //NTL::to_long(rep(coeff(poly, i)));
		}

		return c;
	}
} // namespace Givaro

// Namespace in which all LinBox library code resides
namespace LinBox
{

	class NTL_ZZ_pE_Initialiser {
	public :
		NTL_ZZ_pE_Initialiser( const Integer & p, size_t k ) {
			// linbox_check(e == 1);
			// if ( q > 0 )
			// NTL::ZZ_p::init(NTL::to_ZZ((std::string(q)).data())); // it's an error if q not prime, e not 1
			NTL::ZZ_p::init(NTL::to_ZZ(std::string(p).data()));
			NTL::ZZ_pX irredPoly = NTL::BuildIrred_ZZ_pX ((long) k);
			NTL::ZZ_pE::init(irredPoly);

		}

		// template <class ElementInt>
		// NTL_ZZ_pE_Initialiser(const ElementInt& d) {
			// NTL::ZZ_p::init (NTL::to_ZZ(d));
		// }

		// NTL_ZZ_pE_Initialiser (const NTL::ZZ& d) {
			// NTL::ZZ_p::init(d);
		// }

	};



	/**
	 * @brief Wrapper of ZZ_pE from NTL
	 * Define a parameterized class to handle easily Givaro::ZRing<NTL::ZZ_pE> field
	 */
	class NTL_ZZ_pE : public NTL_ZZ_pE_Initialiser, public Givaro::UnparametricOperations<NTL::ZZ_pE> {
	public:
		typedef NTL::ZZ_pE Element ;
		typedef Givaro::UnparametricOperations<Element> Father_t ;

		const Element zero,one,mOne ;


		NTL_ZZ_pE (const integer &p, const int32_t &k) :
			NTL_ZZ_pE_Initialiser(p,k),Father_t ()
			,zero( NTL::to_ZZ_pE(0)),one( NTL::to_ZZ_pE(1)),mOne(-one)


		{

				}


	bool isZero (const Element& a) const
	{
		return NTL::IsZero(a);
	}


	bool isOne (const Element& a) const
	{
		return NTL::IsOne(a);
	}

	bool isMOne (const Element& x) const
		{
			Element y ; neg(y,x);
			return isOne(y);
		}

	bool isUnit (const Element& x) const
        {
            if (deg(rep(x))==0) {
                NTL::ZZ_p u(rep(x)[0]);
                NTL::ZZ d;
                return !NTL::InvModStatus(d,rep(u),NTL::ZZ_p::modulus());
            } else
                return false;
        }

	integer& characteristic (integer &c) const
	{
		std::stringstream s;
		s << NTL::ZZ_p::modulus();
		s >> c;
		return c;
		//return c=static_cast<integer>(to_long(NTL::ZZ_p::modulus()));
	}


	integer& cardinality(integer& c) const
	{
		std::stringstream s;
		s << NTL::ZZ_p::modulus();
		s >> c;
		c=pow(c,int64_t(Element::degree()));
		return c;
	}

	Element& inv(Element& x, const Element& y) const
	{
		x=NTL::to_ZZ_pE(1)/y;
		return x;
	}

	Element& invin(Element& x) const
	{
		x=NTL::to_ZZ_pE(1)/x;
		return x;
	}


	std::istream& read(std::istream& is, Element& x) const
	{
		long tmp = 0;
		is>>tmp;
		x=NTL::to_ZZ_pE(tmp);
		return is;
	}



// #ifdef __LINBOX_XMLENABLED


//	bool toTag(LinBox::Writer &W) const
//	{
//		std::string s;
//		NTL::ZZ_pX poly = Element::modulus();
//		long i;

//		W.setTagName("field");
//		W.setAttribute("implDetail", "ntl-ZZpE");
//		W.setAttribute("cardinality", LinBox::Writer::numToString(s, _card));

//		W.addTagChild();
//		W.setTagName("finite");

//		W.addTagChild();
//		W.setTagName("characteristic");
//		W.addNum(_p);
//		W.upToParent();

//		W.addTagChild();
//		W.setTagName("extension");
//		W.addNum(deg(poly) + 1);
//		W.upToParent();

//		W.addTagChild();
//		W.setTagName("polynomial");

//		for(i = 0; i <= deg(poly); ++i) {
//			W.addNum(coeff(poly, i));
//		}
//		W.upToParent();
//		W.upToParent();
//		W.upToParent();

//		return true;
//	}


//	std::ostream &write(std::ostream &os) const
//	{
//		LinBox::Writer W;
//		if( toTag(W) )
//			W.write(os);

//		return os;
//	}


//	// Elemnt Reading & writing functions
//	// BIG NOTE:  It was decided that for extension fields, the elements
//	// would be represented using a single number that has the following
//	// property:  for an element e in ZZp[x], with e = a0 + a1x + a2x^2 + ...,
//	// represent e as "<cn>n</cn>" where n = a0 + a1 * p + a2 * p^2 + ...
//	//


//	bool toTag(LinBox::Writer &W, const Element &e) const
//	{
//		NTL::ZZ_pX poly = rep(e);
//		NTL::ZZ accum, base = NTL::ZZ_p::modulus();
//		long i;
//		std::string s;

//		accum = 0;
//		for(i = deg(poly); i >= 0; --i) {
//			accum *= base;
//			accum += rep(coeff(poly, i));
//		}


//		W.setTagName("cn");
//		W.addDataChild(LinBox::Writer::numToString(s, accum));

//		return true;
//	}


//	std::ostream &write(std::ostream &os, const Element &e) const
//	{

//		LinBox::Writer W;
//		if( toTag(W, e))
//			W.write(os);

//		return os;
//	}




//	bool fromTag(LinBox::Reader &R, Element &e) const
//	{
//		NTL::ZZ total, base = NTL::ZZ_p::modulus(), rem;
//		std::stringstream ss;

//		if(!R.expectTagName("cn") || !R.expectChildTextNum(total))
//			return false;

//		ss << "[";
//		while(total > 0) {
//			rem = total % base;
//			total /= base;
//			ss << rem;
//			if(total > 0) ss << " ";
//		}

//		ss << "]";

//		ss >> e; // use the extraction stream operator

//		return true;
//	}


//	std::istream &read(std::istream &is, Element &e) const
//	{
//		LinBox::Reader R(is);
//		if( !fromTag(R, e)) {
//			is.setstate(std::istream::failbit);
//			if(!R.initalized()) {
//				is.setstate(std::istream::badbit);
//			}
//		}

//		return is;
//	}


// #endif


	}; // end of class NTL_ZZ_pE






	template <class Ring>
	struct ClassifyRing;

	template<>
	struct ClassifyRing<UnparametricRandIter<NTL::ZZ_pE> > {
		typedef RingCategories::ModularTag categoryTag;
	};



}

namespace LinBox
{


	template<>
	class UnparametricRandIter<NTL::ZZ_pE> {
	public:
		typedef NTL::ZZ_pE Element;
        typedef integer Residu_t;
        UnparametricRandIter(const NTL_ZZ_pE & F ,
                             const uint64_t & seed =0,
                             const Residu_t& size =0) :
            _size(size), _seed(seed), _ring(F) {
			if(_seed == 0)
                NTL::SetSeed(NTL::to_ZZ(static_cast<long unsigned int> (std::time(nullptr))));
			else {
                NTL::SetSeed( Caster<NTL::ZZ,uint64_t>(seed));
			}
		}

        const NTL_ZZ_pE& ring() const { return _ring; }

// #ifdef __LINBOX_XMLENABLED
//		UnparametricRandIter<NTL::ZZ_pE>(LinBox::Reader &R)
//		{
//			if(!R.expectTagName("randiter")) return;
//			if(!R.expectAttributeNum("seed", _seed) || !R.expectAttributeNum("size", _size)) return;

//			if(_seed == 0) _seed = std::time(nullptr);

//			NTL::SetSeed(NTL::to_ZZ(_seed));
//		}
// #endif


		UnparametricRandIter(const UnparametricRandIter<NTL::ZZ_pE>& R) :
            _size(R._size), _seed(R._seed), _ring(R._ring) {
			if(_seed == 0)
                NTL::SetSeed(NTL::to_ZZ(static_cast<long unsigned int>(std::time(nullptr))));
			else
                NTL::SetSeed(Caster<NTL::ZZ,uint64_t>(_seed));
		}

		NTL::ZZ_pE& random (NTL::ZZ_pE& x) const {
			NTL::random(x);
			return x;
		}

	protected:
		Residu_t _size;
		uint64_t _seed;
        const NTL_ZZ_pE& _ring;
	};
}
#endif //__LINBOX_field_ntl_zz_pe_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
