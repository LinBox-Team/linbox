/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/algorithms/cia.h
 * 
 *  Written by Clement Pernet <clement.pernet@imag.fr>
 *
 * See COPYING for license information.
 */

#ifndef __CIA_H
#define __CIA_H

#include "linbox/ring/givaro-polynomial.h"
#include "linbox/field/modular.h"
#include "linbox/randiter/random-prime.h"
#include "linbox/matrix/blas-matrix.h"
#include "linbox/algorithms/blas-domain.h"
#include "linbox/solutions/minpoly.h"

namespace LinBox 
{
	
	/* Algorithm computing the integer characteristic polynomial
	 * of a dense matrix.
	 * See [Dumas-Pernet-Wan ISSAC05]
	 *
	 *
	 */
	template < class Polynomial, class Blackbox >
	Polynomial& cia (Polynomial & P,  Blackbox & A,
			 const Method::BlasElimination  & M) 

	{
		typename Blackbox::Field intRing = A.field();
		typedef Modular<double> Field;
		typedef BlasBlackbox<Field> FBlackbox;
		typedef GivPolynomialRing<typename Blackbox::Field, Dense> IntPolyDom;
		typedef GivPolynomialRing<Field, Dense> FieldPolyDom;
		typedef typename GivPolynomialRing<typename Blackbox::Field, Dense>::Element IntPoly;
		typedef typename GivPolynomialRing<Field, Dense>::Element FieldPoly;

		IntPolyDom IPD(intRing);
		
		FieldPoly fieldCharPoly(A.coldim());
		/* Computation of the integer minimal polynomial */
		IntPoly intMinPoly;
		minpoly (intMinPoly, A, M);
		
		/* Factorization over the integers */
		vector<IntPoly> intFactors;    
		vector<unsigned long> mult;
		IPD.factor (intFactors, mult, intMinPoly);
		size_t nf = intFactors.size();

		/* One modular characteristic polynomial computation */
		RandomPrime primeg (22);
		integer p;
		primeg.randomPrime (p);
		Field F(p);
		FBlackbox * fbb;
		MatrixHom::map<Field,Blackbox> (fbb, A, F);
		charpoly ( fieldCharPoly, *fbb, M);
		/* Determination of the multiplicities */
		FieldPolyDom FPD (F);
		std::vector<FieldPoly> fieldFactors (nf);
		for (size_t i = 0; i < nf; ++i){
			size_t d= intFactors[i].size();
			fieldFactors[i].resize(d);
			for (size_t j = 0; j < d; ++j)
				F.init ((fieldFactors[i])[j], (intFactors[i])[j]);
		}
		
		FieldPoly currPol = fieldCharPoly;
		FieldPoly r,tmp,q;
		std::vector<long> multip (nf);
		for (size_t i = 0; i < nf; ++i) {
			FieldPoly currFact = fieldFactors[i];
			r.clear();
			int m=0;
			q=currPol;
			do{
				currPol = q;
				FPD.divmod (q, r, currPol, currFact);
				m++;
			} while (FPD.isZero (r));
			multip[i] = m-1;
		}
		
		IntPoly intCharPoly (A.coldim());
		intRing.init (intCharPoly[0], 1);
		for (size_t i = 0; i < nf; ++i){
			IPD.pow( P, intFactors[i], multip[i] );
			IPD.mulin( intCharPoly, P );
		}
		return P = intCharPoly;
	}
}

#endif // __CIA_H
