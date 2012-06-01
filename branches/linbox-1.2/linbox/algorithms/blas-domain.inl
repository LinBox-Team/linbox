/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* linbox/algorithms/blas-domain.inl
 * Copyright (C) 2004 Pascal Giorgi, Clément Pernet
 *
 * Written by :
 *               Pascal Giorgi  pascal.giorgi@ens-lyon.fr
 *               Clément Pernet clement.pernet@imag.fr
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
 * Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */



#ifndef __LINBOX_blas_matrix_domain_INL
#define __LINBOX_blas_matrix_domain_INL

#include "linbox/blackbox/blas-blackbox.h"
#include "linbox/matrix/blas-matrix.h"
#include "linbox/matrix/factorized-matrix.h"

namespace LinBox
{

	/*
	 * **********************************************
	 * *** Specialization for BlasBlackbox<Field> ***
	 * **********************************************
	 */


	// Inversion
	// dpritcha: now returns nullity. (2004-07-19)
	// previously returned Ainv but this is passed back anyway.
	template <class Field>
	class BlasMatrixDomainInv<Field,BlasBlackbox<Field> > {
	public:
		int operator() (const Field                   &F,
				BlasBlackbox<Field>        &Ainv,
				const BlasBlackbox<Field>     &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			BlasBlackbox<Field> tmp(A);
			return (*this)(F,Ainv,tmp);
		}

		int operator() (const Field                &F,
				BlasBlackbox<Field>     &Ainv,
				BlasBlackbox<Field>        &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			int nullity;
			FFPACK::Invert(F,A.rowdim(),A.getPointer(),A.getStride(),
				       Ainv.getPointer(),Ainv.getStride(),nullity);
			return nullity;
		}

	};

#ifndef __INTEL_COMPILER
	template <>
#endif
	class BlasMatrixDomainInv<MultiModDouble,BlasBlackbox<MultiModDouble> > {
	public:
		int operator() (const MultiModDouble                   &F,
				BlasBlackbox<MultiModDouble>        &Ainv,
				const BlasBlackbox<MultiModDouble>     &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			BlasBlackbox<MultiModDouble> tmp(A);
			return (*this)(F,Ainv,tmp);
		}

		int operator() (const MultiModDouble                &F,
				BlasBlackbox<MultiModDouble>     &Ainv,
				BlasBlackbox<MultiModDouble>        &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			int nullity, defrank=0;

			for (size_t i=0;i<F.size();++i){
				FFPACK::Invert(F.getBase(i),A.rowdim(), A.getMatrix(i)->getPointer(),A.getMatrix(i)->getStride(),
					       Ainv.getMatrix(i)->getPointer(),Ainv.getMatrix(i)->getStride(),nullity);
				defrank+=nullity;
			}
			return defrank;
		}

	};



	// Rank
	template <class Field>
	class 	BlasMatrixDomainRank<Field,BlasBlackbox<Field> > {
	public:
		inline unsigned int operator() (const Field                &F,
						const BlasBlackbox<Field>  &A) const
		{

			BlasBlackbox<Field> tmp(A);
			return (*this)(F,tmp);
		}

		inline unsigned int operator() (const Field                &F,
						BlasBlackbox<Field>        &A) const
		{

			return FFPACK::Rank(F, A.rowdim(), A.coldim(),A.getPointer(), A.getStride());
		}
	};

	// determinant
	template <class Field>
	class BlasMatrixDomainDet<Field,BlasBlackbox<Field> > {
	public:
		inline typename Field::Element operator()(const Field                 &F,
							  const BlasBlackbox<Field>   &A) const
		{

			BlasBlackbox<Field> tmp(A);
			return  (*this)(F,tmp);
		}

		inline typename Field::Element operator() (const Field                &F,
							   BlasBlackbox<Field>        &A) const
		{

			return FFPACK::Det(F, A.rowdim(), A.coldim(),A.getPointer(), A.getStride());
		}
	};



	/*
	 * **********************************************
	 * *** Specialization for BlasMatrix<Element> ***
	 * **********************************************
	 */


	// Inversion
	// dpritcha: now returns nullity. (2004-07-19)
	// previously returned Ainv but this is passed back anyway.
	template <class Field>
	class BlasMatrixDomainInv<Field,BlasMatrix<typename Field::Element> > {
	public:
		int operator() (const Field                                   &F,
				BlasMatrix<typename Field::Element>        &Ainv,
				const BlasMatrix<typename Field::Element>     &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			BlasMatrix<typename Field::Element> tmp(A);
			return (*this)(F,Ainv,tmp);
		}

		int operator() (const Field                                  &F,
				BlasMatrix<typename Field::Element>       &Ainv,
				BlasMatrix<typename Field::Element>          &A) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == Ainv.rowdim());
			linbox_check( A.coldim() == Ainv.coldim());
			int nullity;
			FFPACK::Invert (F, A.rowdim(), A.getPointer(), A.getStride(),
					Ainv.getPointer(),Ainv.getStride(),nullity);
			return nullity;
		}

	};

	// Rank
	template <class Field>
	class 	BlasMatrixDomainRank<Field,BlasMatrix<typename Field::Element> > {
	public:
		inline unsigned int operator() (const Field                                &F,
						const BlasMatrix<typename Field::Element>  &A) const
		{

			BlasMatrix<typename Field::Element> tmp(A);
			return (*this)(F,tmp);
		}

		inline unsigned int
		operator() (const Field                           &F,
			    BlasMatrix<typename Field::Element>   &A) const
		{

			return (unsigned int) FFPACK::Rank(F, A.rowdim(), A.coldim(),A.getPointer(), A.getStride());
		}
	};

	// determinant
	template <class Field>
	class BlasMatrixDomainDet<Field,BlasMatrix<typename Field::Element> > {
	public:
		inline typename Field::Element operator()(const Field                                &F,
							  const BlasMatrix<typename Field::Element>  &A) const
		{

			BlasMatrix<typename Field::Element> tmp(A);
			return  (*this)(F,tmp);
		}

		inline typename Field::Element operator() (const Field                             &F,
							   BlasMatrix<typename Field::Element>     &A) const
		{

			return FFPACK::Det(F, A.rowdim(), A.coldim(),A.getPointer(), A.getStride());
		}
	};


	/*
	 * specialization for Operand1, Operand2 and Operand3  of type BlasMatrix<Element>
	 */

	template<class Field>
	class 	BlasMatrixDomainAdd<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& A,
								const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.rowdim() == B.rowdim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( A.coldim() == B.coldim());
			linbox_check( C.coldim() == A.coldim());
			FFLAS::fadd (F, C.rowdim(), C.coldim(),
				     A.getPointer(), A.getStride(),
				     B.getPointer(), B.getStride(),
				     C.getPointer(), C.getStride());
			return C;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainCopy<Field,BlasMatrix<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& B,
								const BlasMatrix<typename Field::Element>& A) const
		{
			linbox_check( A.rowdim() == B.rowdim());
			linbox_check( A.coldim() == B.coldim());
			for (size_t i=0; i<A.rowdim(); i++)
				FFLAS::fcopy (F, A.coldim(),
					      B.getPointer() + i*B.getStride(), 1,
					      A.getPointer() + i*A.getStride(), 1);
			return B;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainSub<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& A,
								const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.rowdim() == B.rowdim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( A.coldim() == B.coldim());
			linbox_check( C.coldim() == A.coldim());
			FFLAS::fsub (F, C.rowdim(), C.coldim(),
				     A.getPointer(), A.getStride(),
				     B.getPointer(), B.getStride(),
				     C.getPointer(), C.getStride());
			return C;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainSubin<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( C.rowdim() == B.rowdim());
			linbox_check( C.coldim() == B.coldim());
			FFLAS::fsubin (F, C.rowdim(), C.coldim(),
				     B.getPointer(), B.getStride(),
				     C.getPointer(), C.getStride());
			return C;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainAddin<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( C.rowdim() == B.rowdim());
			linbox_check( C.coldim() == B.coldim());
			FFLAS::faddin (F, C.rowdim(), C.coldim(),
				     B.getPointer(), B.getStride(),
				     C.getPointer(), C.getStride());
			return C;
		}
	};

	//  general matrix-matrix multiplication and addition with scaling
	// D= beta.C + alpha.A*B
	template<class Field>
	class 	BlasMatrixDomainMulAdd<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>&
		operator()(const Field                              & F,
			   BlasMatrix<typename Field::Element>      & D,
			   const typename Field::Element            & beta,
			   const BlasMatrix<typename Field::Element>& C,
			   const typename Field::Element            & alpha,
			   const BlasMatrix<typename Field::Element>& A,
			   const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.coldim() == B.rowdim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( C.coldim() == B.coldim());
			linbox_check( D.rowdim() == C.rowdim());
			linbox_check( D.coldim() == C.coldim());

			D=C;
			// linbox_check(D.getPointer() != C.getPointer());

			// std::cout << "alpha :" << alpha << std::endl;
			// std::cout << "beta  :" << beta  << std::endl;
			// D.write(std::cout << "Dfgem :=" ) <<','<< std::endl;
			// A.write(std::cout << "Afgem :=" ) <<','<< std::endl;
			// B.write(std::cout << "Bfgem :=" ) <<','<< std::endl;
			FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans,
				      C.rowdim(), C.coldim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      B.getPointer(), B.getStride(),
				      beta,
				      D.getPointer(), D.getStride());
			// D.write(std::cout << "Dfgem :=" ) <<','<< std::endl;
			// std::cout << A.getStride() << "," << A.coldim() << std::endl;
			// std::cout << B.getStride() << "," << B.coldim() << std::endl;
			// std::cout << D.getStride() << "," << D.coldim() << std::endl;
			return D;
		}


		BlasMatrix<typename Field::Element>&
		operator() (const Field                              & F,
			    const typename Field::Element            & beta,
			    BlasMatrix<typename Field::Element>      & C,
			    const typename Field::Element            & alpha,
			    const BlasMatrix<typename Field::Element>& A,
			    const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.coldim() == B.rowdim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( C.coldim() == B.coldim());

			FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans,
				      C.rowdim(), C.coldim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      B.getPointer(), B.getStride(),
				      beta,
				      C.getPointer(), C.getStride());
			return C;
		}
	};


	template<class Field>
	class 	BlasMatrixDomainMulAdd<Field,
		BlasMatrix<typename Field::Element>,
		TransposedBlasMatrix<BlasMatrix<typename Field::Element> >,
		BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>&
		operator()(const Field                              & F,
			   BlasMatrix<typename Field::Element>      & D,
			   const typename Field::Element            & beta,
			   const BlasMatrix<typename Field::Element>& C,
			   const typename Field::Element            & alpha,
			   const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& A,
			   const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.getMatrix().rowdim() == B.rowdim());
			linbox_check( C.rowdim() == A.getMatrix().coldim());
			linbox_check( C.coldim() == B.coldim());
			linbox_check( D.rowdim() == C.rowdim());
			linbox_check( D.coldim() == C.coldim());

			D=C;

			FFLAS::fgemm( F, FFLAS::FflasTrans, FFLAS::FflasNoTrans,
				      C.rowdim(), C.coldim(), B.rowdim(),
				      alpha,
				      A.getMatrix().getPointer(), A.getMatrix().getStride(),
				      B.getPointer(), B.getStride(),
				      beta,
				      D.getPointer(), D.getStride());

			return D;
		}


		BlasMatrix<typename Field::Element>&
		operator() (const Field                              & F,
			    const typename Field::Element            & beta,
			    BlasMatrix<typename Field::Element>      & C,
			    const typename Field::Element            & alpha,
			    const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& A,
			    const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.getMatrix().rowdim() == B.rowdim());
			linbox_check( C.rowdim() == A.getMatrix().coldim());
			linbox_check( C.coldim() == B.coldim());

			FFLAS::fgemm( F, FFLAS::FflasTrans, FFLAS::FflasNoTrans,
				      C.rowdim(), C.coldim(), B.rowdim(),
				      alpha,
				      A.getMatrix().getPointer(), A.getMatrix().getStride(),
				      B.getPointer(), B.getStride(),
				      beta,
				      C.getPointer(), C.getStride());
			return C;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainMulAdd<Field,
		BlasMatrix<typename Field::Element>,
		TransposedBlasMatrix<BlasMatrix<typename Field::Element> >,
		TransposedBlasMatrix<BlasMatrix<typename Field::Element> > > {
	public:
		BlasMatrix<typename Field::Element>&
		operator()(const Field                              & F,
			   BlasMatrix<typename Field::Element>      & D,
			   const typename Field::Element            & beta,
			   const BlasMatrix<typename Field::Element>& C,
			   const typename Field::Element            & alpha,
			   const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& A,
			   const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& B) const
		{
			linbox_check( A.getMatrix().rowdim() == B.getMatrix().coldim());
			linbox_check( C.rowdim() == A.getMatrix().coldim());
			linbox_check( C.coldim() == B.getMatrix().rowdim());
			linbox_check( D.rowdim() == C.rowdim());
			linbox_check( D.coldim() == C.coldim());

			D=C;
			// linbox_check(D.getPointer() != C.getPointer());

			FFLAS::fgemm( F, FFLAS::FflasTrans, FFLAS::FflasTrans,
				      C.rowdim(), C.coldim(), A.getMatrix().rowdim(),
				      alpha,
				      A.getMatrix().getPointer(), A.getMatrix().getStride(),
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      beta,
				      D.getPointer(), D.getStride());
			return D;
		}


		BlasMatrix<typename Field::Element>&
		operator() (const Field                              & F,
			    const typename Field::Element            & beta,
			    BlasMatrix<typename Field::Element>      & C,
			    const typename Field::Element            & alpha,
			    const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& A,
			    const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& B) const
		{
			linbox_check( A.getMatrix().rowdim() == B.getMatrix().coldim());
			linbox_check( C.rowdim() == A.getMatrix().coldim());
			linbox_check( C.coldim() == B.getMatrix().rowdim());

			FFLAS::fgemm( F, FFLAS::FflasTrans, FFLAS::FflasTrans,
				      C.rowdim(), C.coldim(), A.getMatrix().rowdim(),
				      alpha,
				      A.getMatrix().getPointer(), A.getMatrix().getStride(),
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      beta,
				      C.getPointer(), C.getStride());
			return C;
		}
	};

	template<class Field>
	class 	BlasMatrixDomainMulAdd<Field,
		BlasMatrix<typename Field::Element>,
		BlasMatrix<typename Field::Element>,
		TransposedBlasMatrix<BlasMatrix<typename Field::Element> > > {
	public:
		BlasMatrix<typename Field::Element>&
		operator()(const Field                              & F,
			   BlasMatrix<typename Field::Element>      & D,
			   const typename Field::Element            & beta,
			   const BlasMatrix<typename Field::Element>& C,
			   const typename Field::Element            & alpha,
			   const BlasMatrix<typename Field::Element>& A,
			   const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& B) const
		{
			linbox_check( A.coldim() == B.getMatrix().coldim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( C.coldim() == B.getMatrix().rowdim());
			linbox_check( D.rowdim() == C.rowdim());
			linbox_check( D.coldim() == C.coldim());

			D=C;
			FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasTrans,
				      C.rowdim(), C.coldim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      beta,
				      D.getPointer(), D.getStride());
			return D;
		}


		BlasMatrix<typename Field::Element>&
		operator() (const Field                              & F,
			    const typename Field::Element            & beta,
			    BlasMatrix<typename Field::Element>      & C,
			    const typename Field::Element            & alpha,
			    const BlasMatrix<typename Field::Element>& A,
			    const TransposedBlasMatrix<BlasMatrix<typename Field::Element> >& B) const
		{
			linbox_check( A.coldim() == B.getMatrix().coldim());
			linbox_check( C.rowdim() == A.rowdim());
			linbox_check( C.coldim() == B.getMatrix().rowdim());

			FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasTrans,
				      C.rowdim(), C.coldim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      beta,
				      C.getPointer(), C.getStride());
			return C;
		}
	};

	/*
	 * specialization for Operand1 and Operand3 of type std::vector<Element>
	 * and Operand2 of type BlasMatrix<Element>
	 */

	//  general matrix-vector multiplication and addition with scaling
	// d = beta.c + alpha.A*b
	template<class Field>
	class BlasMatrixDomainMulAdd<Field,std::vector<typename Field::Element>,BlasMatrix<typename Field::Element>,std::vector<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& d,
								  const typename Field::Element& beta,
								  const std::vector<typename Field::Element>& c,
								  const typename Field::Element& alpha,
								  const BlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& b) const
		{
			linbox_check( A.coldim() == b.size());
			linbox_check( c.size()   == b.size());
			linbox_check( d.size()   == c.size());
			d=c;

			FFLAS::fgemv( F, FFLAS::FflasNoTrans,
				      A.rowdim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      &b[0],1,
				      beta,
				      &d[0],1);
			return d;
		}


		std::vector<typename Field::Element>& operator() (const Field& F,
								  const typename Field::Element& beta,
								  std::vector<typename Field::Element>& c,
								  const typename Field::Element& alpha,
								  const BlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& b) const
		{
			linbox_check( A.coldim() == b.size());
			linbox_check( A.rowdim() == c.size()); //fixed: dpritcha

			FFLAS::fgemv( F, FFLAS::FflasNoTrans,
				      A.rowdim(), A.coldim(),
				      alpha,
				      A.getPointer(), A.getStride(),
				      &b[0],1,
				      beta,
				      &c[0],1);
			return c;
		}
	};

	//  general vector-matrix multiplication and addition with scaling
	// d = beta.c + alpha.a*B -- note order of coldim, rowdim passed to fgemv is switched
	template<class Field>
	class BlasMatrixDomainMulAdd<Field,std::vector<typename Field::Element>,std::vector<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& d,
								  const typename Field::Element& beta,
								  const std::vector<typename Field::Element>& c,
								  const typename Field::Element& alpha,
								  const std::vector<typename Field::Element>& a,
								  const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( B.rowdim() == a.size());
			linbox_check( B.coldim() == c.size());
			linbox_check( d.size()   == c.size());
			d=c;

			FFLAS::fgemv( F, FFLAS::FflasTrans,
				      B.rowdim(), B.coldim(),
				      alpha,
				      B.getPointer(), B.getStride(),
				      &a[0],1,
				      beta,
				      &d[0],1);
			return d;
		}


		std::vector<typename Field::Element>& operator() (const Field& F,
								  const typename Field::Element& beta,
								  std::vector<typename Field::Element>& c,
								  const typename Field::Element& alpha,
								  const std::vector<typename Field::Element>& a,
								  const BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( B.rowdim() == a.size());
			linbox_check( B.coldim() == c.size());

			FFLAS::fgemv( F, FFLAS::FflasTrans,
				      B.rowdim(), B.coldim(),
				      alpha,
				      B.getPointer(), B.getStride(),
				      &a[0],1,
				      beta,

				      &c[0],1);
			return c;
		}
	};


	/*
	 * Specialization for Operand1, Operand2  of type BlasMatrix<Element>
	 * and Operand3 of type BlasPermutation
	 */

	// Matrix permutation product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, BlasPermutation<size_t> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& A,
								const BlasPermutation<size_t>& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,BlasPermutation<size_t> >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>, BlasPermutation<size_t>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasPermutation<size_t>& B,
								const BlasMatrix<typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,BlasPermutation<size_t> >()( F, B, C);
		}
	};

	/*
	 * specialization for Operand1, Operand2  of type BlasMatrix<Element> and Operand3 of type TransposedBlasMatrix<BlasPermutation<size_t> >
	 */

	// Matrix permutation product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> > > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& A,
								const TransposedBlasMatrix<BlasPermutation<size_t> >& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,TransposedBlasMatrix<BlasPermutation<size_t> > >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> >,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const TransposedBlasMatrix<BlasPermutation<size_t> >& B,
								const BlasMatrix<typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,TransposedBlasMatrix<BlasPermutation<size_t> > >()( F, B, C);
		}
	};

	/*
	 * specialization for Operand1 of type BlasMatrix<Element> and Operand2 of type BlasPermutation
	 */

	// In-place matrix permutation product
	template<class Field>
	class BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>, BlasPermutation<size_t> > {
	public:
		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 BlasMatrix<typename Field::Element>& A,
								 const BlasPermutation<size_t>& B) const
		{
			if (B.isIdentity()) return A ;
			linbox_check( A.coldim() >= B.getSize() );
			FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasNoTrans,
					A.rowdim(), 0,(int) B.getOrder(),
					A.getPointer(), A.getStride(), B.getPointer() );
			return A;
		}

		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 const BlasPermutation<size_t>& B,
								 BlasMatrix<typename Field::Element>& A) const
		{
			if (B.isIdentity()) return A ;
			linbox_check( A.rowdim() >= B.getSize() );
			FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasNoTrans,
					A.coldim(), 0,(int) B.getOrder(), A.getPointer(), A.getStride(), B.getPointer() );
			return A;
		}

	};

	template<class Field>
	class BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> > > {
	public:
		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 BlasMatrix<typename Field::Element>& A,
								 const TransposedBlasMatrix<BlasPermutation<size_t> >& B) const
		{
			if (B.getMatrix().isIdentity()) return A ;
			linbox_check( A.coldim() >= B.getMatrix().getSize() );
			FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasTrans,
					A.rowdim(), 0,(int) B.getMatrix().getOrder(),
					A.getPointer(), A.getStride(), B.getMatrix().getPointer() );
			return A;
		}
		BlasMatrix<typename Field::Element>& operator()(  const Field& F,
								  const TransposedBlasMatrix<BlasPermutation<size_t> >& B,
								  BlasMatrix<typename Field::Element>& A) const
		{
			if (B.getMatrix().isIdentity()) return A ;
			linbox_check( A.rowdim() >= B.getMatrix().getSize() );
			FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasTrans,
					A.coldim(), 0,(int) B.getMatrix().getOrder(), A.getPointer(), A.getStride(), B.getMatrix().getPointer() );
			return A;
		}
	};



	/*
	 * specialization for Operand1, Operand2  of type std::vector<Element> and Operand3 of type BlasPermutation
	 */

	// Matrix permutation product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,std::vector< typename Field::Element>,std::vector< typename Field::Element>, BlasPermutation<size_t> > {
	public:
		std::vector< typename Field::Element>& operator()(const Field& F,
								  std::vector< typename Field::Element>& C,
								  const std::vector< typename Field::Element>& A,
								  const BlasPermutation<size_t>& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>,BlasPermutation<size_t> >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,std::vector< typename Field::Element>, BlasPermutation<size_t>,std::vector< typename Field::Element> > {
	public:
		std::vector< typename Field::Element>& operator()(const Field& F,
								  std::vector< typename Field::Element>& C,
								  const BlasPermutation<size_t>& B,
								  const std::vector< typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>,BlasPermutation<size_t> >()( F, B, C);
		}
	};

	/*
	 * specialization for Operand1, Operand2  of type std::vector<Element> and Operand3 of type TransposedBlasMatrix<BlasPermutation<size_t> >
	 */

	// Matrix permutation product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,std::vector< typename Field::Element>,std::vector< typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> > > {
	public:
		std::vector< typename Field::Element>& operator()(const Field& F,
								  std::vector< typename Field::Element>& C,
								  const std::vector< typename Field::Element>& A,
								  const TransposedBlasMatrix<BlasPermutation<size_t> >& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>,TransposedBlasMatrix<BlasPermutation<size_t> > >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,std::vector< typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> >,std::vector< typename Field::Element> > {
	public:
		std::vector< typename Field::Element>& operator()(const Field& F,
								  std::vector< typename Field::Element>& C,
								  const TransposedBlasMatrix<BlasPermutation<size_t> >& B,
								  const std::vector< typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>,TransposedBlasMatrix<BlasPermutation<size_t> > >()( F, B, C);
		}
	};

	/*
	 * specialization for Operand1 of type std::vector<Element> and Operand2 of type BlasPermutation
	 */

	// In-place matrix permutation product
	template<class Field>
	class BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>, BlasPermutation<size_t> > {
	public:
		std::vector< typename Field::Element>& operator()( const Field& F,
								   std::vector< typename Field::Element>& A,
								   const BlasPermutation<size_t>& B) const
		{
			if (B.isIdentity()) return A ;
			linbox_check( A.size() == B.getSize() );
			FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasNoTrans,
					1, 0,(int) B.getOrder(), &A[0], 1, B.getPointer() );
			return A;
		}

		std::vector< typename Field::Element>& operator()( const Field& F,
								   const BlasPermutation<size_t>& B,
								   std::vector< typename Field::Element>& A) const
		{
			if (B.isIdentity()) return A ;
			linbox_check( A.size() >= B.getSize() );
			FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasNoTrans,
					1, 0,(int) B.getOrder(), &A[0], 1, B.getPointer() );
			return A;
		}

	};

	template<class Field>
	class BlasMatrixDomainMulin<Field,std::vector< typename Field::Element>, TransposedBlasMatrix<BlasPermutation<size_t> > > {
	public:
		std::vector< typename Field::Element>& operator()( const Field& F,
								   std::vector< typename Field::Element>& A,
								   const TransposedBlasMatrix<BlasPermutation<size_t> >& B) const
		{
			if (B.getMatrix().isIdentity()) return A ;
			linbox_check( A.size() >= B.getMatrix().getSize() );
			FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasTrans,
					1, 0,(int) B.getMatrix().getOrder(),
					&A[0], 1, B.getMatrix().getPointer() );
			return A;
		}
		std::vector< typename Field::Element>& operator()(  const Field& F,
								    const TransposedBlasMatrix<BlasPermutation<size_t> >& B,
								    std::vector< typename Field::Element>& A) const
		{
			if (B.getMatrix().isIdentity()) return A ;
			linbox_check( A.size() >= B.getMatrix().getSize() );
			FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasTrans,
					1, 0,(int) B.getMatrix().getOrder(), &A[0], 1, B.getMatrix().getPointer() );
			return A;
		}
	};

	/*
	 * specialization for Operand1 of type BlasMatrix<Element> and Operand2
	 * of type TriangularBlasMatrix<Element>
	 */

	// Matrix/Triangular product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element>, TriangularBlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasMatrix<typename Field::Element>& A,
								const TriangularBlasMatrix<typename Field::Element>& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,TriangularBlasMatrix<typename Field::Element> >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>, TriangularBlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const TriangularBlasMatrix<typename Field::Element>& B,
								const BlasMatrix<typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,TriangularBlasMatrix<typename Field::Element> >()( F, B, C);
		}
	};

	/*
	 * specialization for Operand1 of type BlasMatrix<Element> and Operand2 of type TriangularBlasMatrix<Element>
	 */

	// In-place matrix*triangular matrix product
	template<class Field>
	class BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,
	      TriangularBlasMatrix<typename Field::Element> >{
	public:
		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 BlasMatrix<typename Field::Element>& A,
								 const TriangularBlasMatrix<typename Field::Element>& B) const
		{
			typename Field::Element one;
			F.init(one, 1UL);
			linbox_check( A.coldim() == B.rowdim() );

			FFLAS::ftrmm( F, FFLAS::FflasRight, (FFLAS::FFLAS_UPLO) (B.getUpLo()),
				      FFLAS::FflasNoTrans,(FFLAS::FFLAS_DIAG) (B.getDiag()),
				      A.rowdim(), A.coldim(), one,
				      B.getPointer(), B.getStride(), A.getPointer(), A.getStride() );
			return A;
		}

		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 const TriangularBlasMatrix<typename Field::Element>& B,
								 BlasMatrix<typename Field::Element>& A) const
		{
			linbox_check( B.coldim() == A.rowdim() );
			typename Field::Element one;
			F.init(one, 1UL);
			FFLAS::ftrmm( F, FFLAS::FflasLeft, (FFLAS::FFLAS_UPLO)(B.getUpLo()),
				      FFLAS::FflasNoTrans, (FFLAS::FFLAS_DIAG) (B.getDiag()),
				      A.rowdim(), A.coldim(), one,
				      B.getPointer(), B.getStride(),
				      A.getPointer(), A.getStride() );
			return A;
		}
	};


	/*! @internal In-place matrix*triangular matrix product with transpose.
	 */
	template<class Field>
	class BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,
	      TransposedBlasMatrix<TriangularBlasMatrix<typename Field::Element> > >{
	public:
		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 BlasMatrix<typename Field::Element>& A,
								 const TransposedBlasMatrix< TriangularBlasMatrix<typename Field::Element> >& B) const
		{
			typename Field::Element one;
			F.init(one, 1UL);
			linbox_check( B.getMatrix().coldim() == A.coldim() );

			FFLAS::ftrmm( F, FFLAS::FflasRight,
				      (FFLAS::FFLAS_UPLO)(B.getMatrix().getUpLo()),
				      FFLAS::FflasTrans,
				      (FFLAS::FFLAS_DIAG) (B.getMatrix().getDiag()),
				      A.rowdim(), A.coldim(),
				      one,
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      A.getPointer(), A.getStride() );
			return A;
		}

		BlasMatrix<typename Field::Element>& operator()( const Field& F,
								 const TransposedBlasMatrix< TriangularBlasMatrix< typename Field::Element> >& B,
								 BlasMatrix<typename Field::Element>& A) const
		{
			linbox_check( B.getMatrix().coldim() == A.rowdim() );
			typename Field::Element one;
			F.init(one, 1UL);
			FFLAS::ftrmm( F, FFLAS::FflasLeft,
				      (FFLAS::FFLAS_UPLO) (B.getMatrix().getUpLo()),
				      FFLAS::FflasTrans,
				      (FFLAS::FFLAS_DIAG) (B.getMatrix().getDiag()),
				      A.rowdim(), A.coldim(), one,
				      B.getMatrix().getPointer(), B.getMatrix().getStride(),
				      A.getPointer(), A.getStride() );
			return A;
		}
	};



	/*
	 * specialization for Operand1 of type TriangularBlasMatrix<Element> and Operand2 of type BlasPermutation
	 */

	// Matrix permutation product C = A*B
	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>,TriangularBlasMatrix<typename Field::Element>, BlasPermutation<size_t> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const TriangularBlasMatrix<typename Field::Element>& A,
								const BlasPermutation<size_t>& B) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,BlasPermutation<size_t> >()( F, C, B);
		}
	};

	template<class Field>
	class BlasMatrixDomainMul<Field,BlasMatrix<typename Field::Element>, BlasPermutation<size_t>,TriangularBlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator()(const Field& F,
								BlasMatrix<typename Field::Element>& C,
								const BlasPermutation<size_t>& B,
								const TriangularBlasMatrix<typename Field::Element>& A) const
		{
			C = A;
			return BlasMatrixDomainMulin<Field,BlasMatrix<typename Field::Element>,BlasPermutation<size_t> >()( F, B, C);
		}
	};

	/*
	 * Specialization for Operand of type BlasMatrix<Element>
	 */

	template <class Field>
	class BlasMatrixDomainLeftSolve<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 BlasMatrix<typename Field::Element>& X,
								 const BlasMatrix<typename Field::Element>& A,
								 const BlasMatrix<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.left_solve(X,B);
			return X;
		}


		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 const BlasMatrix<typename Field::Element>& A,
								 BlasMatrix<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.left_solve(B);
			return B;
		}
	};

	template <class Field>
	class BlasMatrixDomainRightSolve<Field,BlasMatrix<typename Field::Element>,BlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 BlasMatrix<typename Field::Element>& X,
								 const BlasMatrix<typename Field::Element>& A,
								 const BlasMatrix<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.right_solve(X,B);
			return X;
		}


		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 const BlasMatrix<typename Field::Element>& A,
								 BlasMatrix<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.right_solve(B);
			return B;
		}

	};

	/*
	 * Specialization for Operand of type std::vector<Element>
	 */

	template <class Field>
	class BlasMatrixDomainLeftSolve<Field, std::vector<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& X,
								  const BlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.left_solve(X,B);
			return X;
		}

		std::vector<typename Field::Element>& operator()(const Field& F,
								 const BlasMatrix<typename Field::Element>& A,
								 std::vector<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.left_solve(B);
			return B;
		}

	};

	template <class Field>
	class BlasMatrixDomainRightSolve<Field, std::vector<typename Field::Element>, BlasMatrix<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& X,
								  const BlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.right_solve(X,B);
			return X;
		}

		std::vector<typename Field::Element>& operator() (const Field& F,
								  const BlasMatrix<typename Field::Element>& A,
								  std::vector<typename Field::Element>& B) const
		{
			LQUPMatrix<Field> LQUP(F,A);
			LQUP.right_solve(B);
			return B;
		}

	};


	/*
	 * ********************************************************
	 * *** Specialization for TriangularBlasMatrix<Element> ***
	 * ********************************************************
	 */


	/*
	 * specialization for Operand of type BlasMatrix<Element>
	 */

	template <class Field>
	class BlasMatrixDomainLeftSolve<Field, BlasMatrix<typename Field::Element>,TriangularBlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 BlasMatrix<typename Field::Element>& X,
								 const TriangularBlasMatrix<typename Field::Element>& A,
								 const BlasMatrix<typename Field::Element>& B) const
		{

			linbox_check( X.rowdim() == B.rowdim());
			linbox_check( X.coldim() == B.coldim());

			typename BlasMatrix<typename Field::Element>::ConstIterator  Biter =   B.Begin();
			typename BlasMatrix<typename Field::Element>::Iterator       Xiter =   X.Begin();

			for (; Biter != B.End(); ++Biter,++Xiter)
				F.assign(*Xiter,*Biter);

			return (*this)(F,A,X);

		}

		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 const TriangularBlasMatrix<typename Field::Element>& A,
								 BlasMatrix<typename Field::Element>& B) const
		{
			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.coldim() == B.rowdim());
			typename Field::Element _One;
			F.init(_One,1UL);

			FFLAS::ftrsm( F,
				      FFLAS::FflasLeft, (FFLAS::FFLAS_UPLO) A.getUpLo(),
				      FFLAS::FflasNoTrans,(FFLAS::FFLAS_DIAG) A.getDiag(),
				      A.rowdim(), B.coldim(),
				      _One,A.getPointer(),A.getStride(),
				      B.getPointer(),B.getStride());

			return B;
		}
	};

	template <class Field>
	class BlasMatrixDomainRightSolve<Field,BlasMatrix<typename Field::Element>, TriangularBlasMatrix<typename Field::Element> > {
	public:
		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 BlasMatrix<typename Field::Element>& X,
								 const TriangularBlasMatrix<typename Field::Element>& A,
								 const BlasMatrix<typename Field::Element>& B) const
		{

			linbox_check( X.rowdim() == B.rowdim());
			linbox_check( X.coldim() == B.coldim());

			typename BlasMatrix<typename Field::Element>::ConstIterator  Biter =   B.Begin();
			typename BlasMatrix<typename Field::Element>::Iterator       Xiter =   X.Begin();

			for (; Biter != B.End(); ++Biter,++Xiter)
				F.assign(*Xiter,*Biter);

			return (*this)(F,A,X);
		}

		BlasMatrix<typename Field::Element>& operator() (const Field& F,
								 const TriangularBlasMatrix<typename Field::Element>& A,
								 BlasMatrix<typename Field::Element>& B) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( B.coldim() == A.rowdim());
			typename Field::Element _One;
			F.init(_One,1UL);

			FFLAS::ftrsm( F,
				      FFLAS::FflasRight,(FFLAS::FFLAS_UPLO) A.getUpLo(),
				      FFLAS::FflasNoTrans,(FFLAS::FFLAS_DIAG) A.getDiag() ,
				      B.rowdim(), A.coldim(),
				      _One,A.getPointer(),A.getStride(),
				      B.getPointer(),B.getStride());


			return B;
		}
	};

	/*
	 * specialization for Operand of type std::vector<Element>
	 */

	template <class Field>
	class BlasMatrixDomainLeftSolve<Field, std::vector<typename Field::Element>, TriangularBlasMatrix<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& x,
								  const TriangularBlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& b) const
		{

			linbox_check (x.size() == b.size());
			typename std::vector<typename Field::Element>::const_iterator biter = b.begin();
			typename std::vector<typename Field::Element>::iterator       xiter = x.begin();
			for (;biter!=b.end();++biter,++xiter)
				F.assign(*xiter,*biter);

			return (*this)(F,A,x);
		}

		std::vector<typename Field::Element>& operator() (const Field& F,
								  const TriangularBlasMatrix<typename Field::Element>& A,
								  std::vector<typename Field::Element>& b) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.rowdim() == b.size());

			switch (A.getUpLo()) {
			case LinBoxTag::Upper:
				switch(A.getDiag()) {
				case LinBoxTag::Unit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasUpper,FFLAS::FflasNoTrans,FFLAS::FflasUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				case LinBoxTag::NonUnit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasUpper,FFLAS::FflasNoTrans,FFLAS::FflasNonUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				default:
					throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");
				}
				break;
			case LinBoxTag::Lower:
				switch(A.getDiag()) {
				case LinBoxTag::Unit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasLower,FFLAS::FflasNoTrans,FFLAS::FflasUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				case LinBoxTag::NonUnit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasLower,FFLAS::FflasNoTrans,FFLAS::FflasNonUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				default:
					throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");
				}
				break;
			default:
				throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");

			}
			return b;
		}
	};

	template <class Field>
	class BlasMatrixDomainRightSolve<Field, std::vector<typename Field::Element>, TriangularBlasMatrix<typename Field::Element> > {
	public:
		std::vector<typename Field::Element>& operator() (const Field& F,
								  std::vector<typename Field::Element>& x,
								  const TriangularBlasMatrix<typename Field::Element>& A,
								  const std::vector<typename Field::Element>& b) const
		{

			linbox_check (x.size() == b.size());
			typename std::vector<typename Field::Element>::const_iterator biter = b.begin();
			typename std::vector<typename Field::Element>::iterator       xiter = x.begin();
			for (;biter!=b.end();++biter,++xiter)
				F.assign(*xiter,*biter);

			return (*this)(F,A,x);
		}

		std::vector<typename Field::Element>& operator() (const Field& F,
								  const TriangularBlasMatrix<typename Field::Element>& A,
								  std::vector<typename Field::Element>& b) const
		{

			linbox_check( A.rowdim() == A.coldim());
			linbox_check( A.coldim() == b.size());


			switch (A.getUpLo()) {
			case LinBoxTag::Upper:
				switch(A.getDiag()) {
				case LinBoxTag::Unit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasUpper,FFLAS::FflasTrans,FFLAS::FflasUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				case LinBoxTag::NonUnit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasUpper,FFLAS::FflasTrans,FFLAS::FflasNonUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				default:
					throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");
				}
				break;
			case LinBoxTag::Lower:
				switch(A.getDiag()) {
				case LinBoxTag::Unit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasLower,FFLAS::FflasTrans,FFLAS::FflasUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				case LinBoxTag::NonUnit:
					FFLAS::ftrsv( F,
						      FFLAS::FflasLower,FFLAS::FflasTrans,FFLAS::FflasNonUnit,
						      b.size(),A.getPointer(),A.getStride(),&b[0],1);
					break;
				default:
					throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");
				}
				break;
			default:
				throw LinboxError ("Error in BlasMatrixDomain (triangular matrix not well defined)");

			}
			return b;
		}
	};

	template< class Field, class Polynomial>
	class BlasMatrixDomainMinpoly< Field, Polynomial, BlasMatrix<typename Field::Element > > {
	public:
		Polynomial& operator() (const Field &F, Polynomial& P, const BlasMatrix<typename Field::Element >& A) const
		{
			commentator.start ("Modular Dense Minpoly ", "MDMinpoly");

			size_t n = A.coldim();
			linbox_check( n == A.rowdim());
			typename Field::Element * X = new typename Field::Element[n*(n+1)];
			size_t *Perm = new size_t[n];
			for ( size_t i=0; i<n; ++i)
				Perm[i] = 0;
			FFPACK::MinPoly<Field,Polynomial>( F, P, n, A.getPointer(), A.getStride(), X, n, Perm);
			commentator.report(Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION) << "minpoly with " << P.size() << " coefficients" << std::endl;

			delete[] Perm;
			delete[] X;
			commentator.stop ("done",NULL,"MDMinpoly");
			return P;
		}
	};

#if !defined(__INTEL_COMPILER) && !defined(__CUDACC__)
	template <>
#endif
	template< class Field,  class ContPol >
	class BlasMatrixDomainCharpoly< Field,  ContPol, BlasMatrix<typename Field::Element > > {
	public:
		ContPol& operator() ( const Field                                	&F,
				      ContPol                     			&P,
				      const BlasMatrix<typename Field::Element > 	&A) const
		{

			size_t n = A.coldim();
			P.clear();
			linbox_check( n == A.rowdim());
			FFPACK::CharPoly( F, P, n, A.getPointer(), A.getStride());
			return P;
		}
	};

} //end of namespace LinBox

#endif // __LINBOX_blas_matrix_domain_INL
