/* -*- mode:C++ -*- */

/* File: scompose.h
 *  Author: Zhendong Wan
 */

/* Implemenatation of EGV and EGV+ algorithm.
 *  Compute the perturbation A+UV and LAR
 */

#ifndef __LINBOX_SCOMPOSE_H__
#define __LINBOX_SCOMPOSE_H__

#include <linbox/util/debug.h>
#include <linbox/blackbox/blackbox-interface.h>
#include <linbox/blackbox/dense.h>
#include <linbox/blackbox/compose.h>

namespace LinBox {
	
	class SCompose : public  BlackboxInterface {
		
	public:
		
		//- general case, composeSmall is the linbox compose
		template<class Blackbox>
			
			//For EGV+ algorithm, using LAR.
			static Compose<Blackbox, Blackbox>*& compose (Compose<Blackbox, Blackbox>*& LAR,
								      const Blackbox& L,
								      const Blackbox& A,
								      const Blackbox& R) {
			
			linbox_check (L.coldim() == A.rowdim());
			
			linbox_check (A.coldim() == R.rowdim());
			
			std::vector<const Blackbox*> VB(3);
			
			VB[0] = &L;
			
			VB[1] = &A;
			
			VB[2] = &R;

			LAR = new Compose<Blackbox, Blackbox>(VB);

			return LAR;			
		}
		
		// specialization for dense matrix case, explicitly compute the LAR by matrix multiplication
		template <class Field>			
			static DenseMatrix<Field>*& compose (DenseMatrix<Field>*& LAR, 
							     const DenseMatrix<Field>& L, 
							     const DenseMatrix<Field>& A, 
							     const DenseMatrix<Field>& R) {
			
			linbox_check (L.coldim() == A.rowdim());
			
			linbox_check (A.coldim() == R.rowdim());
			
			LAR = new DenseMatrix<Field>(L.field(), L.rowdim(), R.coldim());
			
			typename DenseMatrix<Field>::ConstRowIterator crow_p;
			
			typename DenseMatrix<Field>::RowIterator row_p;
			
			std::vector<typename Field::Element> tmp(R.rowdim());
			
			for (row_p = LAR -> rowBegin(), crow_p = L.rowBegin();
			     row_p != LAR -> rowEnd(); ++ row_p, ++ crow_p) {
				
				A.applyTranspose(tmp, *crow_p);
				
				R.applyTranspose(*row_p, tmp);
			}
			
			return LAR;
			
		}
		/*
		//- Compute A + UV, for EGV algorithm, not be used any more.
		template <class Blackbox>			
			static Blackbox*& composeBig (Blackbox*& AUV, 
						      const Blackbox& A, 
						      const Blackbox& U,
						      const Blackbox& V);

			

		// @memo This composeBig creates A + UV for EGV algorithm for the DenseMatrix case. 
		template <class Field>			
			static DenseMatrix<Field>*& composeBig (DenseMatrix<Field>*& AUV, 
							     const DenseMatrix<Field>& A, 
							     const DenseMatrix<Field>& U, 
							     const DenseMatrix<Field>& V) {
			
			linbox_check (U.rowdim() == A.rowdim());
			
			linbox_check (A.coldim() == V.coldim());
			
			AUV = new DenseMatrix<Field>(A.field(), A.rowdim(), A.coldim());
			
			typename DenseMatrix<Field>::ConstRowIterator crow_p;
			
			typename DenseMatrix<Field>::RowIterator row_p;
			
			for (row_p = AUV -> rowBegin(), crow_p = U.rowBegin();
			     row_p != AUV -> rowEnd(); ++ row_p, ++ crow_p) {
				
				V.applyTranspose(*row_p, *crow_p);
				
			}

			typename DenseMatrix<Field>::ConstRawIterator celt_p;
			typename DenseMatrix<Field>::RawIterator elt_p;

			for( elt_p = AUV -> rawBegin(), celt_p = A.rawBegin(); celt_p !=  A.rawEnd(); ++ elt_p, ++ celt_p)
				A.field().addin(*elt_p,*celt_p);
			
			return AUV;
			
		}
		*/

		
	};
}


#endif

