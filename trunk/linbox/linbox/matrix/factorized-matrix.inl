/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* linbox/matrix/factorized-matrix.inl
 * Copyright (C) 2004 Pascal Giorgi, Cl�ment Pernet
 *
 * Written by :
 *               Pascal Giorgi  pascal.giorgi@ens-lyon.fr
 *               Cl�ment Pernet clement.pernet@imag.fr
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __FACTORIZED_MATRIX_INL
#define __FACTORIZED_MATRIX_INL

namespace LinBox{


	// get the Matrix L
	template <class Field,class Matrix>
	inline const BlasMatrix<Matrix>& LQUPMatrix<Field,Matrix>::getL() const {}

	// get the matrix U
	template <class Field,class Matrix>
	inline const BlasMatrix<Matrix>& LQUPMatrix<Field,Matrix>::getU() const { 

		Element zero;
		F.init(zero,0L);
		BlasMatrix<Matrix> tmp(_LU);
		for (int i=0;i<_m;++i)
			for (int j=0;j<i;++j)
				tmp.setEntry(i,j,zero);			
		return *(new BlasMatrix<Matrix> (tmp));
	}

	// get the Matrix S (from the LSP factorization of A deduced from LQUP)
	template <class Field,class Matrix>
	inline const BlasMatrix<Matrix>& LQUPMatrix<Field,Matrix>::getS() const {}



	/*
	 * Solvers with Matrices
	 */
	// solve AX=B
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_solve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}

	// solve AX=B (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_solve(BlasMatrix<Matrix>& B) const{}

	// solve XA=B
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_solve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}

	// solve XA=B (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_solve(BlasMatrix<Matrix>& B) const{}


	// solve LX=B (L from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Lsolve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}
		
	// solve LX=B (L from LQUP) (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Lsolve(BlasMatrix<Matrix>& B) const{}

	// solve XL=B (L from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Lsolve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}
		
	// solve XL=B (L from LQUP) (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Lsolve(BlasMatrix<Matrix>& B) const{}


	// solve UX=B (U from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Usolve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}
		
	// solve UX=B (U from LQUP) (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::rleft_Usolve(BlasMatrix<Matrix>& B) const{}

	// solve XU=B (U from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Usolve(BlasMatrix<Matrix>& X, const BlasMatrix<Matrix>& B) const{}
		
	// solve XU=B (U from LQUP) (X is stored in B)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Usolve(BlasMatrix<Matrix>& B) const{}


	/*
	 * Solvers with vectors
	 */
		
	// solve Ax=b
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_solve(std::vector<Element>& x, const std::vector<Element>& b) const{}
		
	// solve Ax=b (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_solve(std::vector<Element>& b) const{}

	// solve xA=b
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_solve(std::vector<Element>& x, const std::vector<Element>& b) const{}

	// solve xA=b (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_solve(std::vector<Element>& b) const{}


	// solve Lx=b (L from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Lsolve(std::vector<Element>& x, const std::vector<Element>& b) const{}
		
	// solve Lx=b (L from LQUP) (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Lsolve(std::vector<Element>& b) const{}

	// solve xL=b (L from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Lsolve(std::vector<Element>& x, const std::vector<Element>& b) const{}
		
	// solve xL=b (L from LQUP) (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Lsolve(std::vector<Element>& b) const{}


	// solve Ux=b (U from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::left_Usolve(std::vector<Element>& x, const std::vector<Element>& b) const{}
		
	// solve Ux=b (U from LQUP) (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::rleft_Usolve(std::vector<Element>& b) const{}

	// solve xU=b (U from LQUP)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Usolve(std::vector<Element>& x, const std::vector<Element>& b) const{}
		
	// solve xU=b (U from LQUP) (x is stored in b)
	template<class Field, class Matrix> 
	inline bool LQUPMatrix::right_Usolve(std::vector<Element>& b) const{}



} //end of namespace LinBox


#endif
