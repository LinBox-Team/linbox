/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linobx/fflas/fflas.h
 * Copyright (C) 2005 Clement Pernet
 *
 * Written by Clement Pernet <Clement.Pernet@imag.fr>
 *
 * See COPYING for license information.
 */

#ifndef __FFLAS_H
#define __FFLAS_H

#ifndef MAX
#define MAX(a,b) ((a < b)?b:a)
#endif
#ifndef MIN
#define MIN(a,b) ((a > b)?b:a)
#endif

//#include <linbox-config.h>
#include "linbox/field/unparametric.h"
#include <linbox/config-blas.h>

namespace LinBox {
	
	
	//#ifdef __LINBOX_BLAS_AVAILABLE //commented for documentation purposes

#ifndef __LINBOX_STRASSEN_OPTIMIZATION
#define WINOTHRESHOLD 1200
#else
#define WINOTHRESHOLD __LINBOX_WINOTHRESHOLD
#endif

#define DOUBLE_MANTISSA 53
	
	/**
	 * \brief BLAS for matrices over finite fields.
	 * \ingroup fflas

	 *  This class only provides a set of static member functions. No instantiation is allowed.
	 */

	
	class FFLAS {

	public:	
		enum FFLAS_TRANSPOSE { FflasNoTrans=111, FflasTrans=112};
		enum FFLAS_UPLO      { FflasUpper=121, FflasLower=122 };
		enum FFLAS_DIAG      { FflasNonUnit=131, FflasUnit=132 };
		enum FFLAS_SIDE      { FflasLeft=141, FflasRight = 142 };
		
		typedef UnparametricField<double> DoubleDomain;
	
//---------------------------------------------------------------------
// Level 1 routines
//---------------------------------------------------------------------
                //---------------------------------------------------------------------
		// fscal: X <- alpha.X
		// X is a vector of size n
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fscal (const Field& F, const size_t n, const typename Field::Element alpha, 
		       typename Field::Element * X, const size_t incX){
			
			typename Field::Element * Xi = X;
			for (; Xi < X+n*incX; Xi+=incX )
				F.mulin( *Xi, alpha );
		}
		//---------------------------------------------------------------------
		// fcopy: x <- y
		// x,y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fcopy (const Field& F, const size_t N, 
		       typename Field::Element * X, const size_t incX,
		       const typename Field::Element * Y, const size_t incY ){
			
			typename Field::Element * Xi = X;
			const typename Field::Element * Yi=Y;
			for (; Xi < X+N*incX; Xi+=incX, Yi+=incY )
				F.assign(*Xi,*Yi);
		}
		//---------------------------------------------------------------------
		// faxpy: y <- a.x + y
		// x,y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		faxpy (const Field& F, const size_t N, 
		       const typename Field::Element a,
		       const typename Field::Element * X, const size_t incX,
		       typename Field::Element * Y, const size_t incY );
		
		//---------------------------------------------------------------------
		// fdot: returns x^T . y
		// x and y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static typename Field::Element
		fdot (const Field& F, const size_t N, 
		      const typename Field::Element * X, const size_t incX,
		      const typename Field::Element * Y, const size_t incY );
		
		//---------------------------------------------------------------------
		// fswap: X <-> Y
		// X,Y are vectors of size N
		//---------------------------------------------------------------------
		///
		template<class Field>
		static void
		fswap (const Field& F, const size_t N, typename Field::Element * X, const size_t incX,
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
		 *  @brief finite prime Field GEneral Matrix Vector multiplication
		 *
		 *  Computes  Y <- alpha op(A).X + beta.Y \\
		 *  A is m*n
		 */
		template<class Field>
		static void
		fgemv (const Field& F, const enum FFLAS_TRANSPOSE TransA, 
		       const size_t M, const size_t N,
		       const typename Field::Element alpha, 
		       const typename Field::Element * A, const size_t lda,
		       const typename Field::Element * X, const size_t incX, 
		       const  typename Field::Element beta,
		       typename Field::Element * Y, const size_t incY);
		
		/**
		 *  @brief fger: GEneral ?
		 *
		 *  Computes  A <- alpha x . y^T + A \\
		 *  A is m*n, x and y are vectors of size m and n
		 */
		template<class Field>
		static void
		fger (const Field& F, const size_t M, const size_t N,
		      const typename Field::Element alpha, 
		      const typename Field::Element * x, const size_t incx,
		      const typename Field::Element * y, const size_t incy, 
		      typename Field::Element * A, const size_t lda);
		
		/**
		   @brief ftrsv: TRiangular System solve with Vector
		   Computes  X <- op(A^-1).X\\
		   size of X is m
		*/
		template<class Field>
		static void
		ftrsv (const Field& F, const enum FFLAS_UPLO Uplo, 
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
		ftrsm (const Field& F, const enum FFLAS_SIDE Side,
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
		ftrmm (const Field& F, const enum FFLAS_SIDE Side,
		       const enum FFLAS_UPLO Uplo, 
		       const enum FFLAS_TRANSPOSE TransA,
		       const enum FFLAS_DIAG Diag, 
		       const size_t M, const size_t N,
		       const typename Field::Element alpha,
		       typename Field::Element * A, const size_t lda,
		       typename Field::Element * B, const size_t ldb);
	
		/** @brief  Field GEneral Matrix Multiply 
		 * 
		 * Computes C = alpha.op(A)*op(B) + beta.C ,
		 * op(A) = A, A<sup>T</sup>
		 * wl recursive levels of Winograd's algorithm are used 
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
		       const size_t wl);
	
		/** @brief  Field GEneral Matrix Multiply 
		 * 
		 * Computes C = alpha.op(A)*op(B) + beta.C ,
		 * op(A) = A, A<sup>T</sup>
		 * Automitically set Winograd recursion level
		 */
		template<class Field>
		static typename Field::Element* fgemm (const Field& F,
						       const enum FFLAS_TRANSPOSE ta,
						       const enum FFLAS_TRANSPOSE tb,
						       const size_t m,
						       const size_t n,
						       const size_t k,
						       const typename Field::Element alpha,
						       const typename Field::Element* A, 
						       const size_t lda,
						       const typename Field::Element* B,
						       const size_t ldb, 
						       const typename Field::Element beta,
						       typename Field::Element* C, 
						       const size_t ldc){
			size_t ws =0;
			if ( (ta==FflasNoTrans)  && (tb==FflasNoTrans)) {
				size_t kt = MIN(MIN(k,m),n);
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
		fsquare (const Field& F,
			 const enum FFLAS_TRANSPOSE ta,
			 const size_t n,
			 const typename Field::Element alpha,
			 const typename Field::Element* A, 
			 const size_t lda,
			 const typename Field::Element beta,
			 typename Field::Element* C, 
			 const size_t ldc);
	protected:

		// Prevents the instantiation of the class
		FFLAS(){}
		template <class X,class Y>
		class AreEqual
		{
		public:
			static const bool value = false;
		};
	
		template <class X>
		class AreEqual<X,X>
		{
		public:
			static const bool value = true;
		};
	
		//-----------------------------------------------------------------------------
		// Some conversion functions
		//-----------------------------------------------------------------------------
	
		//---------------------------------------------------------------------
		// Finite Field matrix => double matrix
		//---------------------------------------------------------------------
		template<class Field>
		static void MatF2MatD (const Field& F,
				       DoubleDomain::Element* S, const size_t lds,
				       const typename Field::Element* E,
				       const size_t lde,const size_t m, const size_t n){
		
			const typename Field::Element* Ei = E;
			DoubleDomain::Element *Si=S;
			size_t j; 
			for (; Ei < E+lde*m; Ei+=lde, Si += lds)
				for ( j=0; j<n; ++j){
					F.convert(*(Si+j),*(Ei+j));
				}
		}
	
		//---------------------------------------------------------------------
		// Finite Field matrix => double matrix
		// Special design for upper-triangular matrices
		//---------------------------------------------------------------------
		template<class Field>
		static void MatF2MatD_Triangular (const Field& F,
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
		template<class Field>
		static void MatD2MatF (const Field& F,
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
	
		template <class Field>
		static size_t FflasKmaxCompute (const Field& F, const size_t w,
						const typename Field::Element& beta);
		template <class Field>
		static size_t FflasKmax( const Field& F, const size_t w,
					 const typename Field::Element& beta );
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
		static void MatVectProd (const Field& F, 
					 const enum FFLAS_TRANSPOSE TransA, 
					 const size_t M, const size_t N,
					 const typename Field::Element alpha, 
					 const typename Field::Element * A, const size_t lda,
					 const typename Field::Element * X, const size_t incX, 
					 const typename Field::Element beta,
					 typename Field::Element * Y, const size_t incY);


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
		static void WinoCalc (const Field& F, 
				      const enum FFLAS_TRANSPOSE ta,
				      const enum FFLAS_TRANSPOSE tb,
				      const size_t mr, const size_t nr,const size_t kr,
				      const typename Field::Element alpha,
				      const typename Field::Element* A,const size_t lda,
				      const typename Field::Element* B,const size_t ldb,
				      const typename Field::Element beta,
				      typename Field::Element * C, const size_t ldc,
				      const size_t kmax, const size_t w);
	
		template<class Field>
		static void WinoMain (const Field& F, 
				      const enum FFLAS_TRANSPOSE ta,
				      const enum FFLAS_TRANSPOSE tb,
				      const size_t m, const size_t n, const size_t k,
				      const typename Field::Element alpha,
				      const typename Field::Element* A,const size_t lda,
				      const typename Field::Element* B,const size_t ldb,
				      const typename Field::Element beta,
				      typename Field::Element * C, const size_t ldc,
				      const size_t kmax, const size_t w);
		template<bool AreEq>
		class callWinoMain;

		template<bool AreEq>
		class callClassicMatmul;

		template<bool AreEq>
		class callFsquare;

		template<bool AreEq>
		class callMatVectProd;
		

		// Specialized routines for ftrsm
		template<class Field>
		static void ftrsmLeftUpNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						const typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb);
	
		template<class Field>
		static void ftrsmLeftUpTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element alpha,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb);
	
		template<class Field>
		static void ftrsmLeftLowNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element alpha,
						 typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb, 
						 const size_t nmax);
		template<bool AreEq>
		class callFtrsmLeftLowNoTrans;
		

		template<bool AreEq>
		class callFtrsmRightUpNoTrans;
		
		template<bool AreEq>
		class callFtrmmLeftUpNoTrans;

		template<bool AreEq>
		class callFtrmmLeftUpTrans;

		template<bool AreEq>
		class callFtrmmLeftLowNoTrans;		

		template<bool AreEq>
		class callFtrmmLeftLowTrans;		

		template<bool AreEq>
		class callFtrmmRightUpNoTrans;		

		template<bool AreEq>
		class callFtrmmRightUpTrans;		

		template<bool AreEq>
		class callFtrmmRightLowNoTrans;		

		template<bool AreEq>
		class callFtrmmRightLowTrans;		
//		template<class Field>
// 		static void ftrsmLeftLowNoTrans_dbl (const Field& F, const enum FFLAS_DIAG Diag, 
// 						     const size_t M, const size_t N,
// 						     const typename Field::Element alpha,
// 						     typename Field::Element * A, const size_t lda,
// 						     typename Field::Element * B, const size_t ldb, 
// 						     const size_t nmax);
// 		template<class Field>
// 		static void ftrsmLeftLowNoTrans_gen (const Field& F, const enum FFLAS_DIAG Diag, 
// 						     const size_t M, const size_t N,
// 						     const typename Field::Element alpha,
// 						     typename Field::Element * A, const size_t lda,
// 						     typename Field::Element * B, const size_t ldb, 
//						     const size_t nmax);
		template<class Field>
		static void ftrsmLeftLowTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);
	
		
		template<class Field>
		static void ftrsmRightUpNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element alpha,
						 typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb,
						 const size_t nmax);


		template<class Field>
		static void ftrsmRightUpTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element alpha,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmRightLowNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						  const size_t M, const size_t N,
						  const typename Field::Element alpha,
						  const typename Field::Element * A, const size_t lda,
						  typename Field::Element * B, const size_t ldb);

		template<class Field>
		static void ftrsmRightLowTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element alpha,
						const typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb);

		// Specialized routines for ftrmm
		template<class Field>
		static void ftrmmLeftUpNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, 
						const size_t nmax);


		template<class Field>
		static void ftrmmLeftUpTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					      const size_t M, const size_t N,
					      const typename Field::Element * A, const size_t lda,
					      typename Field::Element * B, const size_t ldb,
					      const size_t nmax);

		template<class Field>
		static void ftrmmLeftLowNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb, 
						 const size_t nmax);

		template<class Field>
		static void ftrmmLeftLowTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb,
					       const size_t nmax);
	
		template<class Field>
		static void ftrmmRightUpNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						 const size_t M, const size_t N,
						 const typename Field::Element * A, const size_t lda,
						 typename Field::Element * B, const size_t ldb, 
						 const size_t nmax);

		template<class Field>
		static void ftrmmRightUpTrans (const Field& F, const enum FFLAS_DIAG Diag, 
					       const size_t M, const size_t N,
					       const typename Field::Element * A, const size_t lda,
					       typename Field::Element * B, const size_t ldb, 
					       const size_t nmax);

		template<class Field>
		static void ftrmmRightLowNoTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						  const size_t M, const size_t N,
						  const typename Field::Element * A, const size_t lda,
						  typename Field::Element * B, const size_t ldb,
						  const size_t nmax);


		template<class Field>
		static void ftrmmRightLowTrans (const Field& F, const enum FFLAS_DIAG Diag, 
						const size_t M, const size_t N,
						const typename Field::Element * A, const size_t lda,
						typename Field::Element * B, const size_t ldb, 
						const size_t nmax);
	}; // class FFLAS
}
#include "fflas_fgemm.inl"
#include "fflas_fgemv.inl"
#include "fflas_fger.inl"
#include "fflas_ftrsm.inl"
#include "fflas_ftrmm.inl"
#include "fflas_ftrsv.inl"
#include "fflas_faxpy.inl"
#include "fflas_fdot.inl"

#endif // __FFLAS_H

	