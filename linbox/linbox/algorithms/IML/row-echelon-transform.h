
#ifndef __LINBOX_algorithms_iml_row_echelon_transform_H
#define __LINBOX_algorithms_iml_row_echelon_transform_H

#include "linbox/matrix/dense-matrix.h"
#include "linbox/matrix/matrix-domain.h"
#include <fflas-ffpack/fflas/fflas.h>


namespace LinBox{ namespace iml{

	//! @bug use BlasPermutation instead of std::vectors
	template<class FField>
	class RowEchelonTransform {
	public:
		typedef typename FField::Element Element;

	private:

		FField               _field;//!@bug non ref ?
		BlasMatrix<FField>   _echlA;
		//! @bug use BlasPermutation
		std::vector<size_t>  _permQ; //P
		std::vector<size_t>  _rowPf; //rp
		Element              _det;
		bool                 _red;


		size_t reduce_rec( BlasMatrix<FField> & A, size_t m1, size_t m2, size_t k,
				 const size_t ks, size_t frows, size_t lrows, size_t redflag,
				 size_t eterm,
				 std::vector<size_t> & P, std::vector<size_t>&rp,
				 Element & d);

	public:

		RowEchelonTransform(const BlasMatrix<FField> &A);
		RowEchelonTransform(BlasMatrix<FField> &A);


		/*!
		 * Calling Sequence:
		 *   RowEchelonTransform(p, A, n, m, frows, lrows, redflag, eterm, Q, rp, d)
		 *
		 * Summary:
		 *   Compute a mod p row-echelon transform of a mod p input matrix
		 *
		 * Description:
		 *   Given a n x m mod p matrix A, a row-echelon transform of A is a 4-tuple
		 *   (U,P,rp,d) with rp the rank profile of A (the unique and strictly
		 *   increasing list [j1,j2,...jr] of column indices of the row-echelon form
		 *   which contain the pivots), P a permutation matrix such that all r leading
		 *   submatrices of (PA)[0..r-1,rp] are nonsingular, U a nonsingular matrix
		 *   such that UPA is in row-echelon form, and d the determinant of
		 *   (PA)[0..r-1,rp].
		 *
		 *   Generally, it is required that p be a prime, as inverses are needed, but
		 *   in some cases it is possible to obtain an echelon transform when p is
		 *   composite. For the cases where the echelon transform cannot be obtained
		 *   for p composite, the function returns an error indicating that p is
		 *   composite.
		 *
		 *   The matrix U is structured, and has last n-r columns equal to the last n-r
		 *   columns of the identity matrix, n the row dimension of A.
		 *
		 *   The first r rows of UPA comprise a basis in echelon form for the row
		 *   space of A, while the last n-r rows of U comprise a basis for the left
		 *   nullspace of PA.
		 *
		 *   For efficiency, this function does not output an echelon transform
		 *   (U,P,rp,d) directly, but rather the expression sequence (Q,rp,d).
		 *   Q, rp, d are the form of arrays and pointers in order to operate inplace,
		 *   which require to preallocate spaces and initialize them. Initially,
		 *   Q[i] = i (i=0..n), rp[i] = 0 (i=0..n), and *d = 1. Upon completion, rp[0]
		 *   stores the rank r, rp[1..r] stores the rank profile. i<=Q[i]<=n for
		 *   i=1..r. The input Matrix A is modified inplace and used to store U.
		 *   Let A' denote the state of A on completion. Then U is obtained from the
		 *   identity matrix by replacing the first r columns with those of A', and P
		 *   is obtained from the identity matrix by swapping row i with row Q[i], for
		 *   i=1..r in succession.
		 *
		 *   Parameters flrows, lrows, redflag, eterm control the specific operations
		 *   this function will perform. Let (U,P,rp,d) be as constructed above. If
		 *   frows=0, the first r rows of U will not be correct. If lrows=0, the last
		 *   n-r rows of U will not be correct. The computation can be up to four
		 *   times faster if these flags are set to 0.
		 *
		 *   If redflag=1, the row-echelon form is reduced, that is (UPA)[0..r-1,rp]
		 *   will be the identity matrix. If redflag=0, the row-echelon form will not
		 *   be reduced, that is (UPA)[1..r,rp] will be upper triangular and U is unit
		 *   lower triangular. If frows=0 then redflag has no effect.
		 *
		 *   If eterm=1, then early termination is triggered if a column of the
		 *   input matrix is discovered that is linearly dependant on the previous
		 *   columns. In case of early termination, the third return value d will be 0
		 *   and the remaining components of the echelon transform will not be correct.
		 *
		 * Input:
		 *         p: FiniteField, modulus
		 *         A: 1-dim Double array length n*m, representation of a n x m input
		 *            matrix
		 *         n: long, row dimension of A
		 *         m: long, column dimension of A
		 *     frows: 1/0,
		 *          - if frows = 1, the first r rows of U will be correct
		 *          - if frows = 0, the first r rows of U will not be correct
		 *          .
		 *     lrows: 1/0,
		 *          - if lrows = 1, the last n-r rows of U will be correct
		 *          - if lrows = 0, the last n-r rows of U will not be correct
		 *          .
		*   redflag: 1/0,
		*          - if redflag = 1, compute row-echelon form
		*          - if redflag = 0, not compute reow-echelon form
		*          .
		*     eterm: 1/0,
		*          - if eterm = 1, terminate early if not in full rank
		*          - if eterm = 0, not terminate early
		*          .
		*         Q: 1-dim long array length n+1, compact representation of
		*            permutation vector, initially Q[i] = i, 0 <= i <= n
		*        rp: 1-dim long array length n+1, representation of rank profile,
		*            initially rp[i] = 0, 0 <= i <= n
		*         d: pointer to FiniteField, storing determinant of the matrix,
		*            initially *d = 1
		*
		* Precondition:
		*   ceil(n/2)*(p-1)^2+(p-1) <= 2^53-1 = 9007199254740991 (n >= 2)
		*
		*/

		void reduce ( const size_t frows, const size_t lrows
			      , const size_t redflag, const size_t eterm, std::vector<size_t>&Q,
			      std::vector<size_t> &rp, Element &d)
		{

			size_t m = _echlA.coldim();
			reduce_rec(_echlA,1, m, 0, 0, frows, lrows, redflag,
				   eterm, Q, rp, d);
			//!@todo simplify args w/ data
			_permQ = Q;
			_rowPf = rp;
			_det = d;
			_red=true;

			return ;
		}

		BlasMatrix<FField> & refRed() { return _echlA ; }
		std::vector<size_t>  getPerm() { return _permQ ; }
		std::vector<size_t>  getProf() { return _rowPf ; }



	}; // RowEchelonTransform

		/*!
		 * Calling Sequence:
		 *   Adj <-- mAdjoint(p, A, n)
		 *
		 * Summary:
		 *   Compute the adjoint of a mod p square matrix
		 *
		 * Description:
		 *   Given a n x n mod p matrix A, the function computes adjoint of A. Input
		 *   A is not modified upon completion.
		 *
		 * Input:
		 *   p: FiniteField, prime modulus
		 *      if p is a composite number, the routine will still work if no error
		 *      message is returned
		 *   A: 1-dim Double array length n*n, representation of a n x n mod p matrix.
		 *      The entries of A are casted from integers
		 *   n: long, dimension of A
		 *
		 * Return:
		 *   1-dim Double matrix length n*n, repesentation of a n x n mod p matrix,
		 *   adjoint of A
		 *
		 * Precondition:
		 *   n*(p-1)^2 <= 2^53-1 = 9007199254740991
		 *
		 */
	template<class Field>
	BlasMatrix<Field> &
	mAdjoint (BlasMatrix<Field> & B, const BlasMatrix<Field> &A)
	{
		Field F = A.field();
		linbox_check(A.coldim()==A.rowdim());
		size_t i, j, k, r, count=0;
		typedef typename Field::Element Element;


		size_t n =  A.rowdim();
		std::vector<size_t> P(n+1), rp(n+1);
		Element det, d;

		for (i = 0; i < n+1; i++) { P[i] = i; }
		d = F.one;
		B = A;
		RowEchelonTransform<Field> RET(B);
		RET.reduce(1, 1, 1, 0, P, rp, d);
		det = d ;
		r = rp[0];
		if (r < n-1) {
			FFLAS::fzero(F,B.rowdim(),B.coldim(),B.getPointer(),B.getStride());
			return B;
		}
		if (r == n) {
			for (i = r; i > 0; i--) {
				if (P[i] != i) {
					++count;
					FFLAS::fswap(F,n,B.getWritePointer()+(i-1),n,B.getWritePointer()+(P[i]-1),n);
				}
			}
			if (count % 2 == 0) {
				FFLAS::fscalin(F,B.rowdim(),B.coldim(),det,B.getPointer(),B.getStride());
			}
			else {
				FFLAS::fscalin(F,B.rowdim(),B.coldim(),-det,B.getPointer(),B.getStride());
			}
			return B;
		}
		else {
			if (n == 1) {
				B.setEntry(0,0,F.one);
				return B;
			}
			for (i = 0; i < n; i++) {
				B.setEntry(i,n-1,F.zero);
			}
			B.setEntry((n-1),(n-1),F.one);
			for (i = r; i > 0; i--) {
				if (P[i] != i) {
					++count;
					FFLAS::fswap(F,n,B.getWritePointer()+(i-1),n,
						     B.getWritePointer()+(P[i]-1),n);
				}
			}
			for (j = 1; j < r+1; j++) { if (j != rp[j]) { break; } }
			std::vector<Element> C(n);
			//!@bug I want to apply to the vector column j-1 of A
			std::vector<Element> Aj(n) ;
			FFLAS::fcopy(F,n,&(Aj[0]),1,A.getPointer()+(j-1),n);
			B.apply(C,Aj);
			FFLAS::fscalin(F,n,-1,&C[0],1);
			for (i = 0; i < n-1; i++) {
				for (k = 0; k < n; k++) {
					B.setEntry(i,k,F.zero);
				}
			}
			FFLAS::fger(F,n-1,n,F.one,B.getWritePointer(),n,&C[0],1,B.getWritePointer()+(n-1)*n,1);
			for (i = n-2; i > j-2; i--) {
				++count;
				FFLAS::fswap(F,n,B.getWritePointer()+(i*n),n,B.getWritePointer()+(i+1)*n,n);
			}
			if (count % 2 == 0){
				FFLAS::fscalin(F,B.rowdim(),B.coldim(),det,B.getPointer(),B.getStride());
			}
			else {
				FFLAS::fscalin(F,B.rowdim(),B.coldim(),-det,B.getPointer(),B.getStride());
			}
			return B;
		}
	}





	/**
	 * Calling Sequence:
	 *   r/-1 <-- mBasis(p, A, n, m, basis, nullsp, B, N)
	 *
	 * Summary:
	 *   Compute a basis for the rowspace and/or a basis for the left nullspace
	 *   of a mod p matrix
	 *
	 * Description:
	 *   Given a n x m mod p matrix A, the function computes a basis for the
	 *   rowspace B and/or a basis for the left nullspace N of A. Row vectors in
	 *   the r x m matrix B consist of basis of A, where r is the rank of A in
	 *   Z/pZ. If r is zero, then B will be NULL. Row vectors in the n-r x n
	 *   matrix N consist of the left nullspace of A. N will be NULL if A is full
	 *   rank.
	 *
	 *   The pointers are passed into argument lists to store the computed basis
	 *   and nullspace. Upon completion, the rank r will be returned. The
	 *   parameters basis and nullsp control whether to compute basis and/or
	 *   nullspace. If set basis and nullsp in the way that both basis and
	 *   nullspace will not be computed, an error message will be printed and
	 *   instead of rank r, -1 will be returned.
	 *
	 * Input:
	 *        p: FiniteField, prime modulus
	 *           if p is a composite number, the routine will still work if no
	 *           error message is returned
	 *        A: 1-dim Double array length n*m, representation of a n x m mod p
	 *           matrix. The entries of A are casted from integers
	 *        n: long, row dimension of A
	 *        m: long, column dimension of A
	 *    basis: 1/0, flag to indicate whether to compute basis for rowspace or
	 *           not
	 *         - basis = 1, compute the basis
	 *         - basis = 0, not compute the basis
	 *   nullsp: 1/0, flag to indicate whether to compute basis for left nullspace
	 *           or not
	 *         - nullsp = 1, compute the nullspace
	 *         - nullsp = 0, not compute the nullspace
	 *
	 * Output:
	 *   B: pointer to (Double *), if basis = 1, *B will be a 1-dim r*m Double
	 *      array, representing the r x m basis matrix. If basis = 1 and r = 0,
	 *      *B = NULL
	 *
	 *   N: pointer to (Double *), if nullsp = 1, *N will be a 1-dim (n-r)*n Double
	 *      array, representing the n-r x n nullspace matrix. If nullsp = 1 and
	 *      r = n, *N = NULL.
	 *
	 * Return:
	 *   - if basis and/or nullsp are set to be 1, then return the rank r of A
	 *   - if both basis and nullsp are set to be 0, then return -1
	 *
	 * Precondition:
	 *   n*(p-1)^2 <= 2^53-1 = 9007199254740991
	 *
	 * Note:
	 *   - In case basis = 0, nullsp = 1, A will be destroyed inplace. Otherwise,
	 *     A will not be changed.
	 *   - Space of B and/or N will be allocated in the function
	 *
	 */

	template<class Field>
size_t
	mBasis ( const BlasMatrix<Field> & A, const size_t basis, const size_t nullsp
		 , BlasMatrix<Field> &B, BlasMatrix<Field> & N)
	{
		typedef typename Field::Element Element;
		Field F = A.field();
		typedef BlasMatrix<Field> Matrix ;
		BlasMatrixDomain<Field> BMD(F);
		size_t n = A.rowdim();
		size_t m = A.coldim();
		size_t i, r;
		Element d;

		std::vector<size_t> P(n+1),rp(n+1);
		for (i = 0; i < n+1; i++) { P[i] = i; }
		d = F.one;
		if ((basis == 1) && (nullsp == 1))
		{
			BlasMatrix<Field> A1(A);
			RowEchelonTransform<Field> RET(A1);
			RET.reduce(1, 1, 1, 0, P, rp, d);
			r = rp[0];
			BlasMatrix<Field> U(F,n,n);
			//!@bug define diagonal matrix
			for (i = 0; i < n; i++) {
				U.setEntry(i,i,F.one);
			}
			if (r != 0) {
				FFLAS::fcopy(F,n,r,U.getPointer(),n,RET.refRed().getPointer(),m);
			}
			for (i = r; i > 0; i--) {
				if (P[i] != i) {
					FFLAS::fswap(F,n,U.getWritePointer()+(i-1),n,
						     U.getWritePointer()+(P[i]-1),n);
				}
			}
			if (r == 0) {
				B.resize(0,0);
			}
			else {
				B.resize(r,m);
				BlasSubmatrix<Matrix> U2(U,0,0,r,n);
				BlasSubmatrix<Matrix> A2(A,0,0,n,m);
				BlasSubmatrix<Matrix> B2(B,0,0,r,m);
				BMD.mul(B2,U2,A2);
			}
			if (r ==n) {
				N.resize(0,0);
			}
			else {
				N.resize(n-r,n);
				FFLAS::fcopy(F,n-r,n,N.getWritePointer(),n,U.getPointer()+r*n,n);
			}
			return r;
		}
		else if ((basis == 1) && (nullsp == 0)) {
			BlasMatrix<Field> A1(A);
			RowEchelonTransform<Field> RET(A1);
			RET.reduce( 1, 0, 1, 0, P, rp, d);
			r = rp[0];
			if (r == 0) {
				B.resize(0,0);
				return r;
			}
			BlasMatrix<Field> U(F,r,n);
			//! @bug A1=rref(A1) ??
			FFLAS::fcopy(F,r,r,U.getPointer(),n,RET.refRed().getPointer(),m);
			for (i = r; i > 0; i--) {
				if (P[i] != i) {
					FFLAS::fswap(F,r,U.getPointer()+(i-1),n,U.getPointer()+(P[i]-1),n);
				}
			}
			B.resize(r,m);
			BlasSubmatrix<Matrix> U2(U,0,0,r,n);
			BlasSubmatrix<Matrix> A2(A,0,0,n,m);
			BlasSubmatrix<Matrix> B2(B,0,0,r,m);
			BMD.mul(B2,U2,A2);

			return r;
		}
		else if ((basis == 0) && (nullsp == 1))
		{
			RowEchelonTransform<Field> RET(A);
			RET.reduce( 0, 1, 1, 0, P, rp, d);
			r = rp[0];
			if (r == n) {
				N.resize(0,0);
				return r;
			}
			N.resize(n-r,n);
			if (r != 0) {
				FFLAS::fcopy(F,n-r,r,N.getWritePointer(),n,A.getPointer()+(r*m),m);
			}
			for (i = 0; i < n-r; i++) {
				N.setEntry(i,r+i,F.one);
			}
			//!@bug use applyP !
			for (i = r; i > 0; i--) {
				if (P[i] != i) {
					FFLAS::fswap(F,n-r,N.getPointer()+(i-1),n,N.getPointer()+(P[i]-1),n);
				}
			}
			return r;
		}
		else {
			throw("In mBasis, both basis and nullsp are zero.");
			return 0;
		}
	}



	/*!
	 * Calling Sequence:
	 *   det <-- mDeterminant(p, A, n)
	 *
	 * Summary:
	 *   Compute the determinant of a square mod p matrix
	 *
	 * Input:
	 *   p: FiniteField, prime modulus
	 *      if p is a composite number, the routine will still work if no error
	 *      message is returned
	 *   A: 1-dim Double array length n*n, representation of a n x n mod p matrix.
	 *      The entries of A are casted from integers
	 *   n: long, dimension of A
	 *
	 * Output:
	 *   det(A) mod p, the determinant of square matrix A
	 *
	 * Precondition:
	 *   ceil(n/2)*(p-1)^2+(p-1) <= 2^53-1 = 9007199254740991 (n >= 2)
	 *
	 * Note:
	 *   A is destroyed inplace
	 *
	 */
	template<class Field>
	typename Field::Element
	mDeterminant (BlasMatrix<Field> & A)
	{
		typedef typename Field::Element Element;
		Field  F = A.field();
		size_t i, count=0;
		size_t n = A.rowdim();
		linbox_check(n==A.coldim());
		std::vector<size_t> P(n+1), rp(n+1);
		Element det, d;

		for (i = 0; i <  n+1; i++) { P[i] = i; }
		F.init(d,F.one);
		RowEchelonTransform<Field> RET(A);
		RET.reduce(0, 0, 0, 1, P, rp, d);
		det = d;
		if (!F.isZero(det))
		{
			for (i = 1; i < n+1; i++) {
				if (P[i] != i)
					++count;
			}
			if (count % 2 == 0) {
				return det;
			}
			else
			{
				return F.negin(det);
			}
		}
		return det;
	}


	/*!
	 * Calling Sequence:
	 *   1/0 <-- mInverse(p, A, n)
	 *
	 * Summary:
	 *   Certified compute the inverse of a mod p matrix inplace
	 *
	 * Description:
	 *   Given a n x n mod p matrix A, the function computes A^(-1) mod p
	 *   inplace in case A is a nonsingular matrix in Z/Zp. If the inverse does
	 *   not exist, the function returns 0.
	 *
	 *   A will be destroyed at the end in both cases. If the inverse exists, A is
	 *   inplaced by its inverse. Otherwise, the inplaced A is not the inverse.
	 *
	 * Input:
	 *   p: FiniteField, prime modulus
	 *      if p is a composite number, the routine will still work if no error
	 *      message is returned
	 *   A: 1-dim Double array length n*n, representation of a n x n mod p matrix.
	 *      The entries of A are casted from integers
	 *   n: long, dimension of A
	 *
	 * Return:
	 *   - 1, if A^(-1) mod p exists
	 *   - 0, if A^(-1) mod p does not exist
	 *
	 * Precondition:
	 *   ceil(n/2)*(p-1)^2+(p-1) <= 2^53-1 = 9007199254740991 (n >= 2)
	 *
	 * Note:
	 *   A is destroyed inplace
	 *
	 */

	template<class Field>
	int
	mInverseIn (BlasMatrix<Field> & A)
	{
		typedef typename Field::Element Element;
		Field F = A.field();
		size_t i;
		size_t n = A.rowdim();//,m=A.coldim(); // ...m is not used
		std::vector<size_t> P(n+1),rp(n+1);
		Element d=F.one;

		for (i = 0; i < n+1; i++) { P[i] = i; }
		RowEchelonTransform<Field> RET(A);
		RET.reduce(1, 1, 1, 0, P, rp, d);
		A.copy(RET.refRed()); //!@bug this interface is buggy.
		if (rp[0] == n) {
			for (i = n; i > 0; i--)
				if (P[i] != i){
					FFLAS::fswap(F,n,A.getWritePointer()+(i-1),n,A.getWritePointer()+(P[i]-1),n);
				}
			return 1;
		}
		return 0;
	}


	/*!
	 * Calling Sequence:
	 *   r <-- mRank(p, A, n, m)
	 *
	 * Summary:
	 *   Compute the rank of a mod p matrix
	 *
	 * Input:
	 *   p: FiniteField, prime modulus
	 *      if p is a composite number, the routine will still work if no
	 *      error message is returned
	 *   A: 1-dim Double array length n*m, representation of a n x m mod p
	 *      matrix. The entries of A are casted from integers
	 *   n: long, row dimension of A
	 *   m: long, column dimension of A
	 *
	 * Return:
	 *   r: long, rank of matrix A
	 *
	 * Precondition:
	 *   ceil(n/2)*(p-1)^2+(p-1) <= 2^53-1 = 9007199254740991 (n >= 2)
	 *
	 * Note:
	 *   A is destroyed inplace
	 *
	 */

	template<class Field>
size_t
	mRank (BlasMatrix<Field> &A)
	{
		typedef typename Field::Element Element;
		Field F = A.field();

		size_t i, r;
		size_t n = A.rowdim();//,m=A.coldim(); // m unused
		std::vector<size_t> P(n+1),rp(n+1);
		Element d=F.one;

		for (i = 0; i < n+1; i++) { P[i] = i; }
		d = 1;
		RowEchelonTransform<Field> RET(A);
		RET.reduce( 0, 0, 0, 0, P, rp, d);
		r = rp[0];
		return r;
	}




	/*!
	 * Calling Sequence:
	 *   rp <-- mRankProfile(p, A, n, m)
	 *
	 * Summary:
	 *   Compute the rank profile of a mod p matrix
	 *
	 * Input:
	 *   p: FiniteField, prime modulus
	 *      if p is a composite number, the routine will still work if no
	 *      error message is returned
	 *   A: 1-dim Double array length n*m, representation of a n x m mod p
	 *      matrix. The entries of A are casted from integers
	 *   n: long, row dimension of A
	 *   m: long, column dimension of A
	 *
	 * Return:
	 *   rp: 1-dim long array length n+1, where
	 *     - rp[0] is the rank of matrix A
	 *     - rp[1..r] is the rank profile of matrix A
	 *
	 * Precondition:
	 *   ceil(n/2)*(p-1)^2+(p-1) <= 2^53-1 = 9007199254740991 (n >= 2)
	 *
	 * Note:
	 *   A is destroyed inplace
	 *
	 */

	template<class Field>
	std::vector<size_t>
	mRankProfile (BlasMatrix<Field>&A)
	{
		typedef typename Field::Element Element;
		Field F = A.field();

		size_t i;
		size_t n = A.rowdim(),m=A.coldim();
		std::vector<size_t> P(n+1),rp(n+1);
		Element d=F.one;

		for (i = 0; i < n+1; i++) { P[i] = i; }
		d = F.one;
		RowEchelonTransform<Field> RET(A);
		RET.reduce(0, 0, 0, 0, P, rp, d);
		return rp;

	}



} // iml
} // LinBox

#include "row-echelon-transform.inl"

#endif // __LINBOX_algorithms_iml_row_echelon_transform_H

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
