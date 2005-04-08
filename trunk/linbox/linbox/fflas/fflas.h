/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/fflas/fflas.h
 * Copyright (C) 2003 Clement Pernet
 *
 * Written by Clement Pernet <Clement.Pernet@imag.fr>
 *
 * TRANSPOSE options will force classic matrix multiplication
 *
 * See COPYING for license information.
 */

#ifndef __FFLAS_H
#define __FFLAS_H


#include "linbox/field/unparametric.h"

extern "C" {
#include "cblas.h"
}

namespace LinBox {
	
	
	//#ifdef __LINBOX_BLAS_AVAILABLE //commented for documentation purposes
#define WINOTHRESHOLD 1200
	
	/**
	 * \brief BLAS for matrices over finite fields.
	 * \ingroup fflas
	 * @doc
	 *  This class only provides a set of static member functions. No instantiation is allowed.
	 */
	class FFLAS {
		
		
	public:
		enum FFLAS_TRANSPOSE { FflasNoTrans=111, FflasTrans=112};
		enum FFLAS_UPLO      { FflasUpper=121, FflasLower=122 };
		enum FFLAS_DIAG      { FflasNonUnit=131, FflasUnit=132 };
		enum FFLAS_SIDE      { FflasLeft=141, FflasRight = 142 };
		
		typedef UnparametricField<double> DoubleDomain;
		
		
		//-----------------------------------------------------------------------------
		// Some conversion functions
		//-----------------------------------------------------------------------------
		
		//---------------------------------------------------------------------
		// Finite Field matrix => double matrix
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void MatF2MatD( const Field& F,
				       DoubleDomain::Element* S, const size_t lds,
				       const typename Field::Element* E,
				       const size_t lde,const size_t m, const size_t n){
			
			const typename Field::Element* Ei = E;
			DoubleDomain::Element *Si=S;
			size_t j; //i = 0;
			for (; Ei < E+lde*m; Ei+=lde, Si += lds)
				for ( j=0; j<n; ++j){
					F.convert(*(Si+j),*(Ei+j));
				}
		}
		
		//---------------------------------------------------------------------
		// Finite Field matrix => double matrix
		// Special design for upper-triangular matrices
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void MatF2MatD_Triangular( const Field& F,
						  typename DoubleDomain::Element* S, const size_t lds,
						  const typename Field::Element* const E,
						  const size_t lde,
						  const size_t m, const size_t n){
			
			const typename Field::Element* Ei = E;
			typename DoubleDomain::Element* Si = S;
			size_t i=0, j;
			for ( ; i<m;++i, Ei+=lde, Si+=lds)
				for ( j=i; j<n;++j)
					F.convert(*(Si+j),*(Ei+j));
		}
		
		//---------------------------------------------------------------------
		// double matrix => Finite Field matrix
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void MatD2MatF(  const Field& F,
					typename Field::Element* S, const size_t lds,
					const typename DoubleDomain::Element* E, const size_t lde,
					const size_t m, const size_t n){
			
			typename Field::Element* Si = S;
			const DoubleDomain::Element* Ei =E;
			size_t j;
			for ( ; Si < S+m*lds; Si += lds, Ei+= lde){
				for ( j=0; j<n;++j)
					F.init( *(Si+j), *(Ei+j) );
			}
		}
		
//---------------------------------------------------------------------
// Level 1 routines
//---------------------------------------------------------------------
		//---------------------------------------------------------------------
		// fscal: 
		// Computes  X <- alpha.X
		// X is a vector of size n
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fscal( const Field& F, const size_t n, const typename Field::Element alpha, 
		       typename Field::Element * X, const size_t incX){
			
			typename Field::Element * Xi = X;
			for (; Xi < X+n*incX; Xi+=incX )
				F.mulin( *Xi, alpha );
		}
		//---------------------------------------------------------------------
		// fcopy: 
		// Performs  x <- y
		// x,y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fcopy( const Field& F, const size_t N, 
		       typename Field::Element * X, const size_t incX,
		       const typename Field::Element * Y, const size_t incY ){
		
			typename Field::Element * Xi = X;
			const typename Field::Element * Yi=Y;
			for (; Xi < X+N*incX; Xi+=incX, Yi+=incY )
				F.assign(*Xi,*Yi);
		}
		//---------------------------------------------------------------------
		// faxpy: 
		// Performs  y <- a.x + y
		// x,y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		faxpy( const Field& F, const size_t N, 
		       const typename Field::Element a,
		       const typename Field::Element * X, const size_t incX,
		       typename Field::Element * Y, const size_t incY );

		//---------------------------------------------------------------------
		// fdot:
		// returns x^T . y
		// x and y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static typename Field::Element
		fdot( const Field& F, const size_t N, 
		      const typename Field::Element * X, const size_t incX,
		      const typename Field::Element * Y, const size_t incY );

		//---------------------------------------------------------------------
		// fswap: 
		// Performs X <-> Y
		// X,Y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fswap( const Field& F, const size_t N, typename Field::Element * X, const size_t incX,
		       typename Field::Element * Y, const size_t incY ){
		
			typename Field::Element tmp;
			typename Field::Element * Xi = X;
			typename Field::Element * Yi=Y;
			for (; Xi < X+N*incX; Xi+=incX, Yi+=incY ){
				F.assign( tmp, *Xi );
				F.assign( *Xi, *Yi );
				F.assign( *Yi, tmp );
			}
		}
		
		
//---------------------------------------------------------------------
// Level 2 routines
//---------------------------------------------------------------------
		/**
		 *  @doc fgemv: GEneral Matrix Vector multiplication
		 *
		 *  Computes  Y <- alpha op(A).X + beta.Y \\
		 *  A is m*n
		 */
		template<class Field>
		static void
		fgemv( const Field& F, const enum FFLAS_TRANSPOSE TransA, 
		       const size_t M, const size_t N,
		       const typename Field::Element alpha, 
		       const typename Field::Element * A, const size_t lda,
		       const typename Field::Element * X, const size_t incX, 
		       const  typename Field::Element beta,
		       typename Field::Element * Y, const size_t incY);

		/**
		 *  @doc fger: GEneral ?
		 *
		 *  Computes  A <- alpha x . y^T + A \\
		 *  A is m*n, x and y are vectors of size m and n
		 */
		template<class Field>
		static void
		fger( const Field& F, const size_t M, const size_t N,
		      const typename Field::Element alpha, 
		      const typename Field::Element * x, const size_t incx,
		      const typename Field::Element * y, const size_t incy, 
		      typename Field::Element * A, const size_t lda);

		/**
		   @memo ftrsv: TRiangular System solve with Vector
		   @doc
		   Computes  X <- op(A^-1).X\\
		   size of X is m
		*/
		template<class Field>
		static void
		ftrsv(const Field& F, const enum FFLAS_UPLO Uplo, 
		      const enum FFLAS_TRANSPOSE TransA, const enum FFLAS_DIAG Diag,
		      const size_t N,const typename Field::Element * A, const size_t lda,
		      typename Field::Element * X, int incX);

		//---------------------------------------------------------------------
		// Level 3 routines
		//---------------------------------------------------------------------

		//---------------------------------------------------------------------
		// ftrsm: TRiangular System solve with matrix
		// Computes  B <- alpha.op(A^-1).B,  B <- alpha.B.op(A^-1)
		// B is m*n
		//---------------------------------------------------------------------
		template<class Field>
		static void
		ftrsm(const Field& F, const enum FFLAS_SIDE Side,
		      const enum FFLAS_UPLO Uplo, 
		      const enum FFLAS_TRANSPOSE TransA,
		      const enum FFLAS_DIAG Diag, 
		      const size_t M, const size_t N,
		      const typename Field::Element alpha,
		      typename Field::Element * A, const size_t lda,
		      typename Field::Element * B, const size_t ldb);
	
		//---------------------------------------------------------------------
		// ftrmm: TRiangular Matrix Multiply
		// Computes  B <- alpha.op(A).B,  B <- alpha.B.op(A)
		// B is m*n
		//---------------------------------------------------------------------
		template<class Field>
		static void
		ftrmm(const Field& F, const enum FFLAS_SIDE Side,
		      const enum FFLAS_UPLO Uplo, 
		      const enum FFLAS_TRANSPOSE TransA,
		      const enum FFLAS_DIAG Diag, 
		      const size_t M, const size_t N,
		      const typename Field::Element alpha,
		      typename Field::Element * A, const size_t lda,
		      typename Field::Element * B, const size_t ldb);
	
      

		/** @memo
		 * fgemm: GEneral Matrix Multiply over a Field
		 * @doc
		 * Computes C = alpha.op(A)*op(B) + beta.C\\ 
		 * op(A) = A, A^T\\
		 * Winograd's fast algorithm is used if possible
		 * (when alpha = 1, beta = 0, no transpose.  For now..)
		 */
		template<class Field>
		static typename Field::Element* 
		fgemm( const Field& F,
		       const enum FFLAS_TRANSPOSE ta,
		       const enum FFLAS_TRANSPOSE tb,
		       const size_t m,
		       const size_t n,
		       const size_t k,
		       const typename Field::Element alpha,
		       const typename Field::Element* A, const size_t lda,
		       const typename Field::Element* B, const size_t ldb, 
		       const typename Field::Element beta,
		       typename Field::Element* C, const size_t ldc,
		       const size_t winostep);
	
		//---------------------------------------------------------------------
		// fgemm: GEneral Matrix Multiply over a Field
		// Computes C = alpha.op(A)*op(B) + beta.C 
		// op(A) = A, A^T
		// Automitically set Winograd recursion level
		//---------------------------------------------------------------------
		template<class Field>
		static typename Field::Element* 
		fgemm( const Field& F,
		       const enum FFLAS_TRANSPOSE ta,
		       const enum FFLAS_TRANSPOSE tb,
		       const size_t m,
		       const size_t n,
		       const size_t k,
		       const typename Field::Element alpha,
		       const typename Field::Element* A, const size_t lda,
		       const typename Field::Element* B, const size_t ldb, 
		       const typename Field::Element beta,
		       typename Field::Element* C, const size_t ldc){
			
			size_t ws =0;
			if ( (ta==FflasNoTrans)  && (tb==FflasNoTrans)) {
				size_t kt = k;
				while (kt >= WINOTHRESHOLD){
					ws++;
					kt/=2;
				}
			}
			return fgemm(F, ta, tb, m, n, k, alpha, A, lda, B, ldb,
				     beta, C, ldc, ws);
		}
	
		//---------------------------------------------------------------------
		// fsquare: 
		// compute C = alpha. op(A)*op(A) + beta.C over a Field
		// op(A) =A, A^T
		// Avoid the conversion of B 
		//---------------------------------------------------------------------
		template<class Field>
		static typename Field::Element* 
		fsquare( const Field& F,
			 const enum FFLAS_TRANSPOSE ta,
			 const size_t n,
			 const typename Field::Element alpha,
			 const typename Field::Element* A, const size_t lda,
			 const typename Field::Element beta,
			 typename Field::Element* C, const size_t ldc);



	
	protected:

		// Prevent the instantiation of the class
		FFLAS(){}


		template <class Field>
		static size_t FflasKmax( const Field& F, const size_t w,
					 const typename Field::Element beta );
		
		template <class Field>
		static void DynamicPealing( const Field& F, 
					    const enum FFLAS_TRANSPOSE ta,
					    const enum FFLAS_TRANSPOSE tb,
					    const size_t m, const size_t n, const size_t k,
					    const typename Field::Element alpha, 
					    const typename Field::Element* A, const size_t lda,
					    const typename Field::Element* B, const size_t ldb, 
					    const typename Field::Element beta,
					    typename Field::Element* C, const size_t ldc, 
					    const size_t kmax );


		template<class Field>
		static void 
		MatVectProd( const Field& F, 
			     const enum FFLAS_TRANSPOSE TransA, 
			     const size_t M, const size_t N,
			     const typename Field::Element alpha, 
			     const typename Field::Element * A, const size_t lda,
			     const typename Field::Element * X, const size_t incX, 
			     const typename Field::Element beta,
			     typename Field::Element * Y, const size_t incY);

	// Classic multiplication A(n*k) * B(k*m) in C(n*m)
		template <class Field>
		static void ClassicMatmul(const Field& F,  
					  const enum FFLAS_TRANSPOSE ta,
					  const enum FFLAS_TRANSPOSE tb,
					  const size_t m, const size_t n, const size_t k,
					  const typename Field::Element alpha,
					  const typename Field::Element * A, const size_t lda,
					  const typename Field::Element * B, const size_t ldb,
					  const typename Field::Element beta,
					  typename Field::Element * C, const size_t ldc, 
					  const size_t kmax );
  
		// Winograd Multiplication  alpha.A(n*k) * B(k*m) + beta . C(n*m)
	
		// WinoCalc performs the 22 Winograd operations
		template<class Field>
		static void WinoCalc(const Field& F, 
				     const enum FFLAS_TRANSPOSE ta,
				     const enum FFLAS_TRANSPOSE tb,
				     const size_t mr, const size_t nr,const size_t kr,
				     const typename Field::Element alpha,
				     const typename Field::Element* A,const size_t lda,
				     const typename Field::Element* B,const size_t ldb,
				     const typename Field::Element beta,
				     typename Field::Element * C, const size_t ldc,
				     long long kmax, const size_t winostep);
	
		template<class Field>
		static void WinoMain(const Field& F, 
				     const enum FFLAS_TRANSPOSE ta,
				     const enum FFLAS_TRANSPOSE tb,
				     const size_t m, const size_t n, const size_t k,
				     const typename Field::Element alpha,
				     const typename Field::Element* A,const size_t lda,
				     const typename Field::Element* B,const size_t ldb,
				     const typename Field::Element beta,
				     typename Field::Element * C, const size_t ldc,
				     long long kmax, const size_t winostep);

		// Specialized routines for ftrsm
		template<class Field>
		static void ftrsmLeftUpNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmLeftUpTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					     const size_t M, const size_t N,
					     const typename Field::Element alpha,
					     const typename Field::Element * A, const size_t lda,
					     typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmLeftLowNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, const size_t nmax);

		template<class Field>
		static void ftrsmLeftLowTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element alpha,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmRightUpNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, const size_t nmax);

		template<class Field>
		static void ftrsmRightUpTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element alpha,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmRightLowNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element alpha,
						 const typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmRightLowTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);

		// Specialized routines for ftrmm
		template<class Field>
		static void ftrmmLeftUpNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrmmLeftUpTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					     const size_t M, const size_t N,
					     const typename Field::Element alpha,
					     const typename Field::Element * A, const size_t lda,
					     typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrmmLeftLowNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, const size_t nmax);

		template<class Field>
		static void ftrmmLeftLowTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element alpha,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrmmRightUpNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, const size_t nmax);

		template<class Field>
		static void ftrmmRightUpTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element alpha,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrmmRightLowNoTrans(const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element alpha,
						 const typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb, const size_t nmax);

		template<class Field>
		static void ftrmmRightLowTrans(const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);

	};

	//#endif // __LINBOX_BLAS_AVAILABLE // commented for documentation purposes

}

#include "linbox/fflas/fflas_fgemm.inl"
#include "linbox/fflas/fflas_fgemv.inl"
#include "linbox/fflas/fflas_fger.inl"
#include "linbox/fflas/fflas_ftrsm.inl"
#include "linbox/fflas/fflas_ftrmm.inl"
#include "linbox/fflas/fflas_ftrsv.inl"
#include "linbox/fflas/fflas_faxpy.inl"
#include "linbox/fflas/fflas_fdot.inl"

#endif // __FFLAS_H

