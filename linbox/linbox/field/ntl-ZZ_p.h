/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/field/ntl.h
 * Copyright (C) 1999-2005 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by W. J. Turner <wjturner@acm.org>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __FIELD_NTL_ZZ_p_H
#define __FIELD_NTL_ZZ_p_H

#include <sys/time.h>
#include <NTL/ZZ_p.h>

#include "linbox/field/unparametric.h"
#include "linbox/randiter/unparametric.h"
#include "linbox-config.h"
#include <linbox/field/field-traits.h>

#ifdef __LINBOX_XMLENABLED

#include "linbox/util/xml/linbox-reader.h"
#include "linbox/util/xml/linbox-writer.h"

#include <iostream>
#include <std::string>

#endif


// Namespace in which all LinBox library code resides
namespace LinBox{ 

	template <class Ring>
	struct ClassifyRing;

	template <>
	struct ClassifyRing<UnparametricField<NTL::ZZ_p> > {
		typedef RingCategories::ModularTag categoryTag;
	};

	/** Initialization of field element from an integer.
	 * Behaves like C++ allocator construct.
	 * This function assumes the output field element x has already been
	 * constructed, but that it is not already initialized.
	 * This done by converting to a std::string : inefficient but correct.
	 * @return reference to field element.
	 * @param x field element to contain output (reference returned).
	 * @param y integer.
	 */
	template <>
	NTL::ZZ_p& UnparametricField<NTL::ZZ_p>::init(NTL::ZZ_p& x, const integer& y) const
	{
		return x = NTL::to_ZZ_p( NTL::to_ZZ( (static_cast<const std::string>(y)).c_str() ) );
	}


	/***************************************************************
         *								
         * @brief Wrapper of zz_p from NTL.	  			
         * Uses nice mod p via floating pt trick.			
         *								
         */		
        struct NTL_ZZ_p: public UnparametricField<NTL::ZZ_p>{
		NTL_ZZ_p(integer p, size_t e = 1) 
			: UnparametricField<NTL::ZZ_p>(p, e)
                {}
            
		NTL::ZZ_p& init(NTL::ZZ_p& x, const integer& y) const
		{ 
			return UnparametricField<NTL::ZZ_p>::init(x,y);
		}

		NTL::ZZ_p& init(NTL::ZZ_p& x, const double& y) const
		{
			double z = fmod(y,NTL::to_double(NTL::ZZ_p::modulus()));
			if (z > 0) z += 0.5;
			else z -= 0.5;
			return x = NTL::to_ZZ_p(static_cast<long>(z)); //rounds towards 0
		}
		
                /** Specialization for NTL::ZZ
                 *
                 * @return reference to field element.
                 * @param x field element to contain output (reference returned)
                 * @param y NTL::ZZ.
                 */
		NTL::ZZ_p& init(NTL::ZZ_p& x, const NTL::ZZ& y) const
		{ 
			return x = NTL::to_ZZ_p( y );
		}
            
                /** Specialization for NTL::ZZ
                 *
                 * @return reference to  NTL::ZZ
                 * @param x  NTL::ZZ to contain output (reference returned).
                 * @param y constant reference to field element.
                 */
		NTL::ZZ& convert(NTL::ZZ& x, const NTL::ZZ_p& y) const
		{ 
			return x = y._ZZ_p__rep;
                }

		/** Conversion of field element to an integer.
		 * This function assumes the output field element x has already been
		 * constructed, but that it is not already initialized.
		 * This done by converting to a std::string : inefficient but correct.
		 * @return reference to integer.
		 * @param x reference to integer to contain output (reference returned).
		 * @param y constant reference to field element.
		 */
		integer& convert(integer& x, const NTL::ZZ_p& y) const
		{ 
			NTL::ZZ iy = y._ZZ_p__rep; 
			
			long nb = NTL::NumBytes(iy);
			unsigned char *txt;
			typedef unsigned char u_char;
			txt = new u_char[nb + 68];
			// 			   if (!txt) Error("out of memory");
			BytesFromZZ(txt, iy, nb);
			
			x = 0;
			for (long i = 0; i < nb; i++) {
				x += LinBox::integer( (unsigned long)txt[i] )<<(8*i) ;
			}
			delete [] txt;
			return x;
		};
		
		double& convert(double& x, const NTL::ZZ_p& y) const
		{ 
			x = NTL::to_double(NTL::rep(y));
			return x;
		}

		template <class ANY> //dpritcha--FIX
		NTL::ZZ_p& init(NTL::ZZ_p& x, const ANY& y) const
		{ return x = NTL::to_ZZ_p(static_cast<const long&>(y)); }

		template <class ANY>
		ANY& convert(ANY& x, const NTL::ZZ_p& y) const
		{ return x = static_cast<ANY>(rep(y)); }
            
        };


  
	//@} doc of NTL_ZZ_p

	/** @name NTL_ZZ_p
	 * @brief Arbitrary precision integers modulus a positive integer.

	 * While NTL allows any integer to serve as the modulus, only prime
	 * moduli yield fields.  Therefore, while arthmetic operations may be
	 * valid for any modulus, only prime moduli are supported in this
	 * implementation.  The primality of the modulus will not be checked, so
	 * it is the programmer's responsibility to supply a prime modulus.
	 * These specializations allow the \Ref{UnparametricField} template class to be
	 * used to wrap NTL's {\tt ZZ\_p} class as a LinBox field.
	 */
	//@{

#ifdef __LINBOX_XMLENABLED
	template <> UnparametricField<NTL::ZZ_p>::UnparametricField(Reader &R)
	{

		NTL::ZZ m;
		Integer base(256);
		long e;
		unsigned char* byteArray;

		if(!R.expectTagName("field") || !R.expectChildTag()) return;
		R.traverseChild();

		if(!R.expectTagName("finite") || !R.expectChildTag()) return;
		R.traverseChild();

		if(!R.expectTagName("characteristic") || !R.expectChildTag()) return;
		R.traverseChild();
		if(!R.expectTagNum(m)) return;
		R.upToParent();


		R.upToParent();
		if(R.getNextChild()) {
			R.traverseChild();

			if(!R.expectTagName("extension") || !R.expectChildTextNum(e)) return;
			if(e > 1) {
				R.setErrorString("Tried to extend prime field.");
				R.setErrorCode(Reader::OTHER);
				return;
			}
			R.upToParent();
			R.getPrevChild();
		}

		R.upToParent();
		NTL::ZZ_p::init(m);

		byteArray = new unsigned char[(size_t) NumBytes(m)];
		BytesFromZZ(byteArray, m, NumBytes(m));

		_p = Integer(0);
		for(long i = NumBytes(m) - 1; i >= 0; --i) {
			_p *= base;
			_p += Integer(byteArray[i]);
		}

		delete [] byteArray;

		_card = _p;

	}
		
#endif
	//@{


	template<>
	UnparametricField<NTL::ZZ_p>::UnparametricField(integer q, size_t e)
	{    
		// no default - allow initialization of ZZ_p directly by user.
		//if(q==0) q=65521;   //set default value to 65521
		if ( q > 0 )
		NTL::ZZ_p::init(NTL::to_ZZ((std::string(q)).data())); // it's an error if q not prime, e not 1
		//
	}


	/** Conversion of field element to an integer.
	 * This function assumes the output field element x has already been
	 * constructed, but that it is not already initialized.
	 * This done by converting to a std::string : inefficient but correct.
	 * @return reference to integer.
	 * @param x reference to integer to contain output (reference returned).
	 * @param y constant reference to field element.
	 */

	template <>
	integer& UnparametricField<NTL::ZZ_p>::convert(integer& x, const NTL::ZZ_p& y) const
 	{ 
		NTL::ZZ iy = y._ZZ_p__rep; 
		
		long nb = NTL::NumBytes(iy);
		unsigned char *txt;
		typedef unsigned char u_char;
		txt = new u_char[nb + 68];
		// 			   if (!txt) Error("out of memory");
		BytesFromZZ(txt, iy, nb);
		
		x = 0;
		for (long i = 0; i < nb; i++) {
			x += LinBox::integer( (unsigned long)txt[i] )<<(8*i) ;
		}
		delete [] txt;
		return x;
	}

	//dpritcha
	template<> 
	double& UnparametricField<NTL::ZZ_p>::convert(double& x, const NTL::ZZ_p& y) const
	{ 
		x = NTL::to_double(NTL::rep(y));
		return x;
	}

	/** Cardinality.
	 * Return integer representing cardinality of the field.
	 * Returns the modulus of the field, which should be prime.
	 * @return integer representing cardinality of the field
	 */
	template <> 
	integer& UnparametricField<NTL::ZZ_p>::cardinality(integer& c) const
	{ return c = static_cast<integer>(to_long(NTL::ZZ_p::modulus())); }

	/** Characteristic.
	 * Return integer representing characteristic of the field.
	 * Returns the modulus of the field, which should be prime.
	 * @return integer representing characteristic of the field.
	 */
	template <> 
	integer& UnparametricField<NTL::ZZ_p>::characteristic(integer& c) const
		//FIXME we shouldn't go thru long here as p may be larger than that.
		// check if NTL has cast ZZp to gmp integers.
	{ return c = static_cast<integer>(to_long(NTL::ZZ_p::modulus())); }

	/** Multiplicative Inverse.
	 * x = 1 / y
	 * This function assumes both field elements have already been
	 * constructed and initialized.
	 * @return reference to x.
	 * @param  x field element (reference returned).
	 * @param  y field element.
	 */
	template <> NTL::ZZ_p& 
	UnparametricField<NTL::ZZ_p>::inv(NTL::ZZ_p& x, const NTL::ZZ_p& y) const
	{ return x = NTL::inv(y); }
 
	/** Zero equality.
	 * Test if field element is equal to zero.
	 * This function assumes the field element has already been
	 * constructed and initialized.
	 * In this specialization, NTL's IsZero function is called.
	 * @return boolean true if equals zero, false if not.
	 * @param  x field element.
	 */
	template <> bool UnparametricField<NTL::ZZ_p>::isZero(const NTL::ZZ_p& x) const
	{ return static_cast<bool>(IsZero(x)); }

	/** One equality.
	 * Test if field element is equal to one.
	 * This function assumes the field element has already been
	 * constructed and initialized.
	 * In this specialization, NTL's IsOne function is called.
	 * @return boolean true if equals one, false if not.
	 * @param  x field element.
	 */
	template <> bool UnparametricField<NTL::ZZ_p>::isOne(const NTL::ZZ_p& x) const
	{ return static_cast<bool>(IsOne(x)); }

	/** Inplace Multiplicative Inverse.
	 * x = 1 / x
	 * This function assumes both field elements have already been
	 * constructed and initialized.
	 * @return reference to x.
	 * @param  x field element (reference returned).
	 */
	template <> NTL::ZZ_p& UnparametricField<NTL::ZZ_p>::invin(NTL::ZZ_p& x) const
	{ return x = NTL::inv(x); }


#ifdef __LINBOX_XMLENABLED
	template <> bool UnparametricField<NTL::ZZ_p>::toTag(Writer &W) const
	{
		std::string s;
		W.setTagName("field");
		W.setAttribute("implDetail", "ntl-ZZp");
		W.setAttribute("cardinality", Writer::numToString(s, NTL::ZZ_p::modulus()));

		W.addTagChild();
		W.setTagName("finite");

		W.addTagChild();
		W.setTagName("characteristic");
		W.addNum(NTL::ZZ_p::modulus());
		W.upToParent();

		W.upToParent();

		return true;
	}


	template <> std::ostream &UnparametricField<NTL::ZZ_p>::write(std::ostream &out) const
	{
		Writer W;
		if( toTag(W))
			W.write(out);
		
		return out;
	}



	template <> bool UnparametricField<NTL::ZZ_p>::toTag(Writer &W, const Element &e) const
	{
		std::string s;
		W.setTagName("cn");
		W.addDataChild(Writer::numToString(s, e));
		
		return true;
	}

	template <> std::ostream &UnparametricField<NTL::ZZ_p>::write(std::ostream &out, const Element &e) const
	{
		Writer W;
		if( toTag(W, e))
			W.write(out);

		return out;
	}



	template <> bool UnparametricField<NTL::ZZ_p>::fromTag(Reader &R, Element &e) const
	{
		if(!R.expectTagName("cn") || !R.expectChildTextNum(e)) return false;

		return true;
	}

	template <> std::istream &UnparametricField<NTL::ZZ_p>::read(std::istream &in, Element &e) const {
		Reader R(in);
		if( !fromTag(R, e)) {
			in.setstate(std::istream::failbit);
			if(!R.initalized())
				in.setstate(std::istream::badbit);
		}
		return in;
	}


#else



	/** Print field.
	 * @return output stream to which field is written.
	 * @param  os  output stream to which field is written.
	 */
	template <> std::ostream& UnparametricField<NTL::ZZ_p>::write(std::ostream& os) const 
	{ 
		return os << "unparameterized field NTL::ZZ_p with p = " 
			  << NTL::ZZ_p::modulus(); 
	}

#endif

	/// Constructor for random field element generator
	template <> 
	UnparametricRandIter<NTL::ZZ_p>::UnparametricRandIter (const UnparametricField<NTL::ZZ_p>& F, 
							       const integer& size, 
							       const integer& seed)
		: _size(size), _seed(seed)
	{
		if (_seed == integer(0)) _seed = integer(time(NULL));
		
		integer cardinality; 
		F.cardinality(cardinality);
		if (_size > cardinality)
			_size = 0;

#ifdef TRACE
		std::cout << "created random generator with size " << _size 
		     << " and seed " << _seed << std::endl;
#endif // TRACE
		
		// Seed random number generator
		NTL::SetSeed(NTL::to_ZZ(static_cast<long>(_seed)));
	}

#ifdef __LINBOX_XMLENABLED
	template <> UnparametricRandIter<NTL::ZZ_p>::UnparametricRandIter(Reader &R) {
		if(!R.expectTagName("randiter")) return;
		if(!R.expectAttributeNum("seed", _seed) || !R.expectAttributeNum("size", _size)) return;

		if(_seed == integer(0)) _seed = integer(time(NULL));

		NTL::SetSeed(NTL::to_ZZ(static_cast<long>(_seed)));
	}
#endif



	/// Random field element creator.
	template <> NTL::ZZ_p& UnparametricRandIter<NTL::ZZ_p>::random(NTL::ZZ_p& x) const
		//		{ return x = static_cast<long>((double(rand())/RAND_MAX)*double(_size)); }
	{
		if (_size == 0) {
			return x = NTL::random_ZZ_p(); 
		}
		else {
			return x = NTL::to_ZZ_p(NTL::RandomBnd(static_cast<long>(_size)));
		}
	}

  

} // namespace LinBox

#endif // __FIELD_NTL_ZZ_p_H
