/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
//----------------------------------------------------------------------
//                FFLAP: Finite Field Fast Linear Algebra Package
//                FFLAP_MinPoly: MinPoly of (A,v)
//                               Also Monte-Carlo MinPoly(A)
//----------------------------------------------------------------------
// by Clement PERNET ( clement.pernet@imag.fr )
// 30/04/2003
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// MinPoly: Compute the minimal polynomial of (A,v) using an LUP 
// factorization of the Krylov Base (v, Av, .., A^kv)
// U must have its first row filled with v
// U must be (n+1)*n
//---------------------------------------------------------------------
template <class Field, class Polynomial>
Polynomial&
FFLAP::MinPoly( const Field& F, Polynomial& minP, const size_t N,
		const typename Field::element *A, const size_t lda,
		typename Field::element* U, size_t ldu, size_t* P){

	typedef typename Field::element elt;
	// nRow is the number of row in the krylov base already computed
	size_t j, k, nNewRow, nRow = 2;
	elt* B = new elt[ N*N ];
	const elt* Ai=A;
	typename Polynomial::iterator it;
	elt* Xi, *Ui, *Bi=B, *L, *Li, *y;
	typename Field::randIter g (F);
	bool KeepOn=true;
	// Creating the Krilov Base matrix X
	elt * X = new elt[(N+1)*N];
#if DEBUG==2
	for (j=0;j<(N+1)*N;j++)
		X[j] = F.zero;
#endif
	// Creating the copy of A, where to compute A^2^i
	// Try memcopy here
	for (; Ai<A+lda*N; Ai+=lda-N)
		for ( j=0; j<N; ++j){
			*(Bi++) = *(Ai++);
		}

	// Picking a non zero vector
	do{
		for (Ui=U, Xi = X; Ui<U+N; ++Ui, ++Xi){
			g.random (*Ui);
		 	*Xi = *Ui;
			if (!F.iszero(*Ui))
				KeepOn = false;
		}
	}while(KeepOn);
	

	nRow = 1;
	size_t nUsedRow = 0;
	// LUP factorization of the Krilov Base Matrix
	
	k = LUdivine_construct(F, FflasUnit, N+1, N, B, N, X, N, U, ldu, P,
			       &nRow, N+1, &nUsedRow );
	delete[] B;
	delete[] X;
	minP.resize(k+1);
	minP[k] = F.one;
	if (k==1 && F.isZero(*(U+ldu))){ // minpoly is X
		return minP;
	}
	// m contains the k first coefs of the minpoly
	elt* mi, *m= new elt[k];
	fcopy( F, k, m, 1, U+k*ldu, 1);
	ftrsv( F, FflasLower, FflasTrans, FflasNonUnit, k, U, ldu, m, 1);
	it = minP.begin();
	for (j=0; j<k; ++j, it++){
		F.neg(*it, m[j]);
	}
	delete[] m;
	return minP;
}
